# Security

OneFit-Engine is a scientific fitting engine, not a hardened multi-user web
application. The HTTP service crosses several high-risk boundaries.

## Current security properties

The implementation currently has:

- no authentication;
- no authorization;
- no user sessions;
- no built-in TLS;
- no application-level request-size or rate limits;
- no sandbox boundary between uploads and the fitting/compilation tools.

The `username` form field is not a login. It selects an existing directory
relative to the service's working directory when that directory exists. Each
request uses a unique child there, retained after the response; deployments
must apply retention and disk-usage limits.

## Native code execution

Model expressions and optional auxiliary C source are transformed into,
compiled as, and executed as native code. A client able to submit a fit can
therefore influence a native compilation and execution pipeline.

Only process trusted models and uploads. Do not offer the current `/fit`
endpoint to anonymous or mutually untrusted clients.

## Shell command construction

The current `POST /fit` implementation builds a shell command line by
interpolating request field values (including the `function` field)
directly into a string, then runs it. This is a real command-injection
surface, not a theoretical one - it is not limited to the "native code
execution" risk above. Requested `download` values are also interpreted as
filesystem paths. These patterns require strict upstream trust and make
direct Internet exposure unsafe regardless of any reverse proxy in front of
it.

Working operations also remove and recreate target directories. Use a
dedicated account and filesystem tree with no unrelated data.

## Archive trust

`archive`/`ar`/`log` replay unzips a stored archive entry's data and
**re-executes the exact command that was originally archived**, as a shell
command. Do not import or replay archives from untrusted sources - treat an
archive `.zip` the same way you'd treat an executable script from an
unknown origin.

## Minimum safe local use

1. Bind to `127.0.0.1`.
2. Run under a dedicated, unprivileged account.
3. Keep source, work, configuration, and logs owned by that account.
4. Accept input only from trusted local processes/users.
5. Retain only required output, and review service logs.
6. Apply OS resource limits where expensive fits could exhaust CPU,
   memory, disk, or process slots.

```bash
onefite start-web-engine --ip=127.0.0.1 --port=8142
```

## Remote deployment requirements

If remote access is unavoidable, place the engine in an isolated VM or
container and front it with a trusted gateway that provides:

- strong authentication and authorization;
- TLS;
- strict upload size/type policy;
- normalized filenames and path enforcement;
- request timeouts and concurrency limits;
- CPU, memory, process, and storage quotas;
- audit logging and log retention;
- network restrictions preventing lateral access;
- disposable or per-job workspaces;
- output allowlisting rather than arbitrary path selection.

A reverse proxy alone does not make generated native code (or shell
interpolation) safe. Isolation and trusted callers remain necessary.

## Administrative commands

`INSTALL`, `onefite upgrade`, `onefite uninstall`, systemd management, and
optional web-server setup invoke external tools and sometimes `sudo`. The
default installer can update system packages, install an SSH/web-server
task, and modify ImageMagick's `policy.xml` (after backing it up) to permit
PS/EPS/PDF formats used by the plotting pipeline. Review scripts and commit
local work before running them - see
[installation side effects](installation.md#important-side-effects).

## Recommended hardening work

Future implementation work should avoid shell construction in favor of
fixed argument arrays, validate and normalize uploads, constrain downloads
to generated artifacts, introduce a versioned job API with structured
errors, and run each job within a disposable, restricted worker.
Authentication should live at a clearly defined gateway or application
boundary.

These are recommendations for future work, not properties of the current
implementation.
