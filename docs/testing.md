# Testing

OneFit-Engine's tests cover Raku modules, conversion, compilation, fitting,
graphs, mixed/hybrid behavior, and optional post-install integration.

## Installed CLI tests

List available tests:

```bash
onefite test list
```

Run the Raku suite without service integration:

```bash
onefite test --no-systemd-daemon --no-post-install
```

Run selected tests by basename, with or without `.rakutest`:

```bash
onefite test 00-use 04-convert --no-post-install
```

Selected tests run concurrently and write combined diagnostics to
`/tmp/tests.log`.

## Post-install tests

`--post-install` is enabled by default for `onefite test`. It always runs a
built-in command-line fit; if `-d`/`--systemd-daemon` is also true and
`systemctl status onefite` reports the service active, or otherwise if a
background service process is found, it *also* sends the same fit to
`127.0.0.1:8142`. Disable both checks for an isolated unit-test run with
`--no-systemd-daemon --no-post-install`.

## Repository test runner

For development, once dependencies and generated paths are configured:

```bash
prove6 --lib t
```

Tests use fixtures under `t/` and generate output under
`/tmp/OneFit-Engine-test`. Some expected logs are architecture-specific.

## Multi-distro Docker sandbox

`test-ofe-in-docker-sandbox.sh` builds and runs OFE inside a disposable
container for each supported distribution, using the per-distro Dockerfiles
in `Dockerfiles/` (`debian`, `ubuntu`, `fedora`, `centos`, `alma`, `arch`,
`suse`). Use it to try an install end-to-end without touching your host, or
to reproduce a platform-specific bug.

```bash
tmpdir=$(mktemp -d) && cd $tmpdir
git clone https://github.com/fitteia/OneFit-Engine.git && cd OneFit-Engine

./test-ofe-in-docker-sandbox.sh build debian   # or: ubuntu fedora centos alma arch suse all
./test-ofe-in-docker-sandbox.sh run debian     # drops you into a shell in the container
```

The container mounts a host work folder at `/home/ofe/work` (override with
`HOST_WORK=/some/dir`, default `$HOME/Downloads/sandbox`) and maps host
port `PORT` (default 8142) to the container's OFE service. Set `KEEP=1` to
keep the container after you exit; otherwise it's removed (`--rm`).

```bash
./test-ofe-in-docker-sandbox.sh shell debian   # attach another shell to a running container
```

**Install OFE inside the container.** The per-distro `Dockerfiles/` provide
a base development image, but not a *complete* OFE dependency set (for
example the Debian/Ubuntu images install `raku` but not `zef`, `swig`, or
`libcgi-pm-perl`). Run the platform pre-install script before `./INSTALL`,
exactly as you would on a bare host - don't skip straight to `./INSTALL
--docker` alone, since `--docker` (which also auto-triggers just from being
in a container, whether you pass it or not) disables `./INSTALL`'s own
package-installation step:

```bash
sudo ./pre-install-OFE-in-Linux.sh
./INSTALL --no-test --no-post-test && onefite service start && ./post-install.sh
```

See [Docker](platform/docker.md) for the full walkthrough, and
[installation](installation.md#do-i-need-to-run-a-pre-install-script-first)
for why the pre-install step matters here.

When you're done, remove the containers and images it created (`docker ps
-a` / `docker images` to find them, or `docker system prune` if you don't
need anything else cached).

## Feature prerequisites

Different tests require different external components:

- Raku module tests require installed/precompiled OFE modules;
- compilation tests require C, Fortran, and Perl development headers, plus
  Minuit;
- plots require Grace and PDF utilities;
- MPEG4 requires Poppler/PDF tools and FFmpeg;
- HDF5 imports require HDF5 tools/libraries;
- service checks require Cro modules, `curl`, and a reachable service;
- systemd checks require an installed unit and a systemd environment.

A platform passing the core tests doesn't mean every optional feature
passes too - report partial support precisely.

## Documentation validation

When changing `bin/onefite`'s embedded POD, validate it the same way the
tool itself would render it:

```bash
raku -c bin/onefite
raku --doc=Text bin/onefite
raku --doc=Man bin/onefite
```

Also check local Markdown links, and keep version/license statements
consistent across `README.md`, `META6.json`, and `LICENSE`.
