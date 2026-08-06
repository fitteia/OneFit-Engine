# Installation

OneFit-Engine installation builds a C/Fortran core, installs Raku modules
and dependencies, installs command wrappers and configuration, and
optionally configures system services.

- [Choose a mode](#choose-a-mode)
- [Before running `./INSTALL`](#before-running-install)
- [Important side effects](#important-side-effects)
- [Platform guides](#platform-guides)
- [Common options](#common-options)
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
- install missing Raku dependencies (`App::Prove6`, `Inline::Perl5`,
  `Readline`, `Cro::HTTP::Router`, `Pod::To::Man`);
- run the Raku test suite and post-install tests;
- update Debian-family packages when `--dpkg` applies;
- install/configure an SSH+web-server task on eligible site installs;
- enable PDF-related ImageMagick policy entries;
- fetch/update the sibling `../minuit` checkout and build it;
- install the man page;
- support up to 250 MINUIT parameters (`--minuit=N` to change).

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
--minuit=COUNT             maximum MINUIT parameter count (default 250)
-d, --systemd-daemon       install and enable a systemd unit
--ip=ADDRESS               service bind address used in generated setup
--port=PORT                service port (default 8142)
--shell                    also install a browser-based shell (shellinabox)
--cernlib                  use the distro's CERNLIB package instead of
                            building MINUIT from source
-u, --to-user               install into the user account instead of site-wide
-m, --merge-site=BRANCH     merge a local model-development branch first
```

Use `./INSTALL --help` for the authoritative, current list.

## CERNLIB Minuit

Since OFE 0.9.0, Minuit is built from source (in a parallel `minuit/` folder
next to the OFE checkout) rather than taken from the Debian `cernlib`
package, so the maximum number of fitting parameters can be raised past the
package's built-in limit with `./INSTALL --minuit=N` (default: 250; `onefite
upgrade` re-uses whatever limit the currently-installed Minuit already has
unless you override it). `etc/OFE/default/makefile` (used to compile
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
