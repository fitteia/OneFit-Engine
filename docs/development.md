# Development

## Repository layout

```text
bin/onefite                 CLI, Cro service, and embedded POD
lib/OneFit*.rakumod         Raku engine implementation
C/core/onefit-3.1/          C fitting core
C/local/                    local model functions and interfaces
C/META-C.json               model metadata
etc/OFE/default/            installed configuration templates
examples/command-line/      integration examples
t/                           Raku tests and fixtures
Dockerfiles/                 distribution test environments
INSTALL                      build/install orchestrator
docs/                        this documentation
```

## Development setup

Use a disposable host/container, or review the installer's side effects
before a site installation (see
[installation side effects](installation.md#important-side-effects)). A
user-space development install can start with:

```bash
./INSTALL --to-user --no-dpkg --no-web-server --no-enable-gs --no-git
```

The exact flags you need depend on whether compilers, Minuit, and Raku
modules are already installed.

## Read-only module checks

```bash
raku -Ilib -e 'use OneFit; say "OneFit loaded"'
jq empty META6.json C/META-C.json
```

Some scripts contain `%OFE-PATH%` placeholders replaced during
installation; running them straight from the source tree isn't always
equivalent to running the installed copy.

## Tests

```bash
prove6 --lib t
onefite test --no-systemd-daemon --no-post-install
```

Run targeted tests while iterating, then the full applicable suite. See
[testing](testing.md).

## CLI changes

When changing a `multi MAIN` signature in `bin/onefite`:

1. update its `#=` help text;
2. update the embedded POD;
3. update only the workflow guides materially affected;
4. render the POD as both Text and Man (`raku --doc=Text`/`--doc=Man
   bin/onefite`, or just `raku -c bin/onefite` for a quick syntax check);
5. test positive, negated-Boolean, short-alias, and invalid-option
   behavior;
6. avoid copying generated `--help` output into Markdown files - link to
   the live command instead.

## HTTP changes

Update `http-api.md`, `service.md`, and `security.md` together. Add route
tests covering content types, invalid input, failure status, cleanup, path
confinement, and authentication-boundary assumptions.

Avoid constructing shell commands from request values - this is a known,
documented weakness of the current `/fit` implementation, not a pattern to
extend. Prefer fixed executable/argument arrays, normalized
server-generated filenames, and allowlisted download artifacts. See
[security](security.md).

## Installer changes

Keep `./INSTALL --help`, the platform guides, and
[installation side effects](installation.md#important-side-effects)
aligned. Test the site/system and `--to-user` paths separately. Defaults
that mutate Git state, packages, services, or security policy need
prominent documentation.

## Version and license

`META6.json` is the machine-readable version source. The project declares
the Artistic License 2.0, with the full text in `LICENSE`. Keep metadata,
license text, and README statements consistent.

## Branches and release workflow

The repository currently has `main`, `dev`, and `unstable` branches -
confirm current maintainer policy before publishing changes, since which
branch is "latest" can shift. Use focused branches, preserve other
contributors' uncommitted work (never force-push over it), and don't mix
generated fit output with source commits.

Before release:

- run the applicable Raku, compile, fit, conversion, plot, and service
  tests;
- validate the Docker environments claimed as supported;
- render the embedded manual (`onefite man`);
- check local documentation links;
- verify version/license consistency;
- test a clean install and an upgrade path.

## Documentation policy

Keep one authoritative home for every fact (CLI behavior in the embedded
POD, installer behavior in `INSTALL`/`installation.md`, HTTP behavior in
`http-api.md`) and mark recommendations separately from current
implementation behavior, the way [security](security.md) does.
