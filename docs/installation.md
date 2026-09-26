# Installation

OneFit-Engine installation builds a C/Fortran core, installs Raku modules
and dependencies, installs command wrappers and configuration, and
optionally configures system services.

- [Choose a mode](#choose-a-mode)
- [Before running `./INSTALL`](#before-running-install)
- [Important side effects](#important-side-effects)
- [Platform guides](#platform-guides)
- [Common options](#common-options)
- [Extensions](#extensions)
- [CERNLIB Minuit](#cernlib-minuit)
- [Verify](#verify)
- [Uninstall](#uninstall)
- [Convenience shell functions](#convenience-shell-functions)

## Choose a mode

| Mode | Intended use | Privileges |
| --- | --- | --- |
| Site installation (default) | Shared workstation or server | Uses `sudo` |
| `--to-user` | Single user, no system-wide install | Avoids most system changes |
| `--wsl` | WSL2 | Disables systemd/web-server/test defaults |
| `--macos` | Native Homebrew-based macOS | Installs Homebrew packages |
| `--docker` | Container installation | Disables systemd/package/test defaults (also auto-detected when running inside a container) |

Platform guides below give the complete, tested command sequence for each
environment - this page covers what every mode has in common.

## Before running `./INSTALL`

Review the live interface (option names and defaults can shift between
versions, so this page intentionally doesn't freeze a copy of it):

```bash
./INSTALL --help
```

The current installer's defaults include:

- compile and install the engine;
- install to the Raku site repository;
- install missing Raku dependencies (`App::Prove6`, `Readline`,
  `Cro::HTTP::Router`, `Pod::To::Man`);
- run the Raku test suite and post-install tests;
- update Debian-family packages when `--dpkg` applies;
- install/configure an SSH+web-server task on eligible site installs;
- enable PDF-related ImageMagick policy entries;
- fetch/update the sibling `../minuit` checkout and build it;
- install the man page;
- support up to 1,000 MINUIT internal parameters and 2,000 external slots
  (`--minuit=N` to change).

The engine itself - Minuit, the OneFit C core (`../C`, onefite-c-code) and
its extensions - is built by onefite-c-code's `tools/engine.pl`, the same
script onefite-go's `doctor --install` uses (onefite-c-code's README,
"Installing the engine safely"). It backs up the installed engine files
first, builds everything, links a test program with every extension, and if
anything fails puts the previous engine back and stops: a failed install or
upgrade leaves a working engine. What was installed is recorded in
`etc/engine.json` (core version and commit, Minuit's commit and parameter
limit, every extension with its repository and commit); an upgrade with no
extension options keeps the extensions recorded there. `perl
../C/tools/engine.pl rollback --c-root ../C --root .` goes back one install.
An older onefite-c-code without `tools/engine.pl` is built the previous way.

After native compilation, `INSTALL` writes `MANIFEST.site`. Unlike the
committed source `MANIFEST`, this generated file inventories the Minuit and
OneFit C libraries, headers, data tables, and helper programs installed in the
current checkout. Helper programs installed outside the checkout are recorded
with absolute paths. The file is machine-specific and intentionally ignored by
Git.

## Important side effects

With default options, `./INSTALL` can:

1. Run `git stash` and `git pull` in the current checkout - **commit or
   stash your own local changes first**, or pass `--no-git`. This is easy
   to get bitten by: it silently stashes uncommitted edits (including to
   `bin/onefite` itself) before building, and running `./INSTALL` again
   later stashes *again* on top, so a `git stash list` can quietly
   accumulate several unrelated stashes. Don't blindly `pop`/`drop` them -
   inspect each with `git stash show -p stash@{N}` first.
2. Clone or update the sibling `../minuit` checkout.
3. On Debian-family systems, run package-manager update/upgrade/autoremove
   and install the packages `./INSTALL` needs directly (independent of any
   pre-install script - see below).
4. Install SSH/web-server `tasksel` tasks on Debian-family site installs.
5. Modify ImageMagick's `policy.xml` (after backing it up) to allow
   PS/EPS/PDF/XPS formats used by the plotting pipeline.
6. Write under `/usr/local`, Raku's site module repository, and (with
   `--systemd-daemon`) systemd unit locations.
7. Invoke `sudo` repeatedly.

Use negated Boolean flags to disable what you don't want, e.g. `--no-git`,
`--no-dpkg`, `--no-web-server`, `--no-enable-gs`, `--no-test`,
`--no-post-test` (Raku also accepts the `--/NAME` spelling, e.g. `--/test`).

### Do I need to run a pre-install script first?

On Debian/Ubuntu, `./INSTALL` alone (default options) can provision most of
what it needs itself, via its own `apt-get install` step - it doesn't
strictly require `pre-install-OFE-in-Linux.sh` to have run first. That said,
**running the pre-install script first is the more robust, tested path**:
it handles cases `./INSTALL`'s own package step doesn't (a `zef` build from
source when the distro's `perl6-zef` package is missing or too old, a
Grace/xmgrace build from source when the distro package is unavailable,
multi-distro dispatch for Fedora/Arch/SUSE, and PATH setup), and it's what
this repository's own `post-install.sh`-based flows assume. Use it unless
you have a specific reason not to.

Inside a container, this isn't optional: `./INSTALL` auto-detects that it's
running in Docker and behaves as if `--docker` were passed, which sets
`--no-dpkg` - it will **not** install system packages for you there. See
[Docker](platform/docker.md).

## Platform guides

- [Debian and Ubuntu](platform/debian-ubuntu.md)
- [WSL2](platform/wsl.md)
- [macOS](platform/macos.md)
- [Docker](platform/docker.md)
- [Fedora, RHEL-family, Arch, and openSUSE](platform/other-linux.md)

## Common options

```text
-a, --alias='ofe onefit'   install additional command aliases for onefite
-b, --bindir=DIR           choose the binary installation directory
--minuit=COUNT             maximum MINUIT internal parameter count (default 1000;
                           MNE is twice this value)
-d, --systemd-daemon       install and enable a systemd unit
--ip=ADDRESS               service bind address used in generated setup
--port=PORT                service port (default 8142)
--shell                    also install a browser-based shell (shellinabox)
--cernlib                  use the distro's CERNLIB package instead of
                            building MINUIT from source
-u, --to-user               install into the user account instead of site-wide
-m, --merge-site=BRANCH     merge a local model-development branch first
--extension=SPEC[,SPEC...]  install extensions: NAME (from onefite-c-code's
                            extensions/registry.json) or NAME=URL[@REF]
--/default-extensions       skip the default extension (public Florence)
--extensions-ref=REF        default git ref for extensions (default main)
--extensions-transport=T    https (default), http or ssh for registry repos
--enable-extensions[=MODE]     the older spelling: install florence; =http or =ssh also
                            picks how registry repositories are reached
--shell-port=PORT           shellinabox port (default 8100)
--cpu=amd64|arm64           CPU the packages are for (default: detected)
-c, --/compile              skip compiling and installing the engine core
-i, --/install              don't install OneFit-Engine as a Raku module
-s, --/to-site              install the Raku module for this user, not
                            site-wide (-u implies it)
-p, --/p6-modules           don't install the required Raku modules
--/man-page                 don't install the man page
-U, --Uninstall             uninstall the OneFit-Engine Raku module
```

`./INSTALL --help` lists every option. Boolean options that default to on
are turned off with `--/NAME` (or `--no-NAME`); see also "Important side
effects" above for `--/git`, `--/dpkg`, `--/web-server`, `--/enable-gs`,
`--/test` and `--/post-test`.

## Extensions

Some models, such as Florence, are kept outside the public `onefite-c-code`
tree in their own repositories (public Florence: `fitteia/onefite-ext-florence`).
The public Florence uses a NAG-free clean-room eigensolver and is licensed
separately under Artistic 2.0. The original NAG-derived implementation is kept
only in a private, license-restricted repository for users who already hold the
relevant NAG license; it is marked non-redistributable and needs your own
access to that repository.

Extensions add model functions without editing the base model library. Each
one is a git repository cloned to `../C/extensions/<name>`, built by
`onefite-c-code`'s own `make extensions` (the same code `onefite-go`'s
`doctor --install` uses, so both runtimes behave alike). `INSTALL` runs it
after the engine is built; it writes `etc/extensions.mk` (link and include
flags that every fit's makefile includes) and `../C/META-CATALOG.json` (the
model catalog `onefite list models` and `onefite help MODEL` read; the base
`META-C.json` is never edited).

- With no options the registry's default extension (public Florence) is
  installed. A default that cannot be fetched or built only prints a warning.
- `--extension NAME` installs another registry entry; `--extension NAME=URL`
  or `NAME=URL@REF` installs one from any git URL (a private repository, your
  own). Several may be given comma-separated. One you name that cannot be
  fetched or built fails the install.
- Two extensions that provide the same functions (for example a public and a
  licensed variant of one model) cannot be installed together; the build says
  so and names both. Remove one folder from `../C/extensions/` and re-run.
- Whatever is in `../C/extensions/` is built, so a folder you place there by
  hand is installed too.

- With `--no-git` nothing is downloaded: each extension you name must already be
  checked out in `../C/extensions/<name>`, and `./INSTALL` only rebuilds them.
- The older `./INSTALL --enable-extensions[=http|https|ssh] --extensions-ref=REF`
  still works and means `--extension=florence` (over that transport).
- The Go/Rust port has the same options: `onefite doctor --install
  --extension NAME[=URL[@REF]]` - see the `onefite-native` `README.md`.

To write your own, see `extensions/README.md` in `onefite-c-code`.

Use `./INSTALL --help` for the authoritative, current list.

## CERNLIB Minuit

Since OFE 0.9.0, Minuit is built from source (in a parallel `minuit/` folder
next to the OFE checkout) rather than taken from the Debian `cernlib`
package, so the maximum number of fitting parameters can be raised past the
package's built-in limit with `./INSTALL --minuit=N` (default: 1000 internal,
2,000 external; `onefite
upgrade` re-uses the limit the currently-installed Minuit was built with -
recorded in `etc/engine.json`, or for an engine installed before
`engine.pl`, read from `minuit/d506cm.inc` - unless you override it). `etc/OFE/default/makefile` (used to compile
user-defined model code) is set up to link against this from-source
`libminuit.a`; to go back to the distro package instead, pass `--cernlib` to
`./INSTALL`/`onefite upgrade`, or edit the `MINUIT` variable in that
makefile by hand.

## Verify

```bash
command -v onefite
onefite --version
onefite path
onefite test --no-systemd-daemon --no-post-install
```

If a systemd service was installed:

```bash
onefite service start --systemd-daemon --ip=127.0.0.1
onefite service PID
onefite service log
```

Read [security](security.md) before binding beyond loopback.

## Uninstall

The installer supports `--Uninstall`/`-U`; the CLI wraps it:

```bash
onefite uninstall
```

This stops the running service, updates the source checkout (`git stash &&
git pull`), and runs `./INSTALL -U`. Review local changes and service state
first - see [important side effects](#important-side-effects) above.

## Convenience shell functions

After installation, a few shell functions make day-to-day use nicer. Add
these to your shell rc file:

```bash
# short alias for onefite (skips any locally-redefined function of that name)
unset -f ofe 2>/dev/null
ofe() { command onefite "$@"; }

# reinstall from the current source tree in one step
onefiteSrcPath="$(onefite path --src 2>/dev/null)"
unset -f iofe 2>/dev/null
iofe() {
    cd "$onefiteSrcPath"
    ./INSTALL --no-test
    cd -
}

# search your shell history for onefite invocations
unset -f ofeh 2>/dev/null
ofeh() { history 0 | grep -E 'onefite|ofe' || return; }
```
