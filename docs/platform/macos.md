# macOS

OneFit-Engine supports a native Homebrew-oriented path and a container
path. Native installation may need platform-specific adjustments to
compilers, Grace, and Raku binary locations.

## Native prerequisites

Install Homebrew, then at minimum:

```bash
brew install rakudo git gcc
```

The repository's helper script provisions the wider native dependency set:

```bash
sudo ./pre-install-OFE-in-MacOS.sh
```

Review the script before running it - it can install Homebrew packages,
Raku tools, and system links.

## Native installation

```bash
git clone https://github.com/fitteia/OneFit-Engine.git && cd OneFit-Engine
./INSTALL --macos
```

The macOS installer mode disables Debian package handling, systemd,
web-server setup, and both the ordinary and post-install test suites. It
installs required Homebrew tools such as Grace, ImageMagick, HDF5,
Poppler, FFmpeg, and PDF utilities when missing.

Verify executable discovery:

```bash
command -v raku zef prove6 onefite xmgrace
onefite --version
```

If `zef`/`prove6` are missing from `/usr/local/bin`, or don't point at the
most recent Rakudo:

```bash
cd /usr/local/bin
sudo ln -s "$(raku -e 'say $*REPO.repo-chain[1].Str.subst("inst#","")')/bin/zef"
sudo ln -s "$(raku -e 'say $*REPO.repo-chain[1].Str.subst("inst#","")')/bin/prove6"
```

## Local service

The macOS mode expects loopback. Start explicitly with:

```bash
onefite service start --ip=127.0.0.1
```

Do not expose the service directly to a LAN - see
[security](../security.md).

## RAM disk

`--use-ramdisk` creates or reuses `/Volumes/RAMDisk` using `hdiutil` and
`diskutil`. Data on it is temporary and consumes memory.

## Docker/Colima alternative

For isolation, install Colima and Docker, then follow the
[Docker guide](docker.md). The container path is generally easier to
reproduce than a native scientific toolchain, and is what this project's
own multi-distro test sandbox uses.
