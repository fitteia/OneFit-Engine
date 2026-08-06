# Debian and Ubuntu

Debian-family systems are the installer's most tested path (Debian 12
"bookworm" and Ubuntu 24.04 "Noble Numbat").

## Prerequisites

```bash
sudo apt-get update
sudo apt-get install -y git raku
```

## Recommended install

```bash
git clone https://github.com/fitteia/OneFit-Engine.git && cd OneFit-Engine
sudo ./pre-install-OFE-in-Linux.sh
./INSTALL --no-test --no-post-test && onefite service start && ./post-install.sh
```

`pre-install-OFE-in-Linux.sh` provisions the full system dependency set
(including `zef`, built from source if the packaged `perl6-zef` is missing
or too old, and Grace/xmgrace built from source if unavailable as a
package) before `./INSTALL` compiles and installs OFE itself. This is the
most robust path and the one this project's own scripts assume.

`./INSTALL` alone (default options, on a plain non-container host) can
*also* provision most Debian packages itself, via its own `apt-get
install` step - so `git clone && cd OneFit-Engine && ./INSTALL` is a
shorter path that generally works too. Use the pre-install script when in
doubt, or if `./INSTALL` reports a missing dependency it didn't provision.

The default site installation may upgrade packages, install SSH/web-server
tasks, change ImageMagick's policy, build Minuit, and run tests - read
[installation side effects](../installation.md#important-side-effects).

For a narrower installation:

```bash
./INSTALL --to-user \
  --no-dpkg --no-web-server --no-enable-gs \
  --no-systemd-daemon
```

Don't disable dependency provisioning unless the required compilers,
plotting tools, HDF5 tools, Raku modules, and Minuit are already available.

## Systemd service

Install or update the service with:

```bash
./INSTALL --systemd-daemon --ip=127.0.0.1
onefite service start --systemd-daemon --ip=127.0.0.1
```

Use loopback unless you've deliberately designed a protected remote
deployment - see [service operation](../service.md) and
[security](../security.md).

## Validate

```bash
onefite --version
onefite test --no-systemd-daemon --no-post-install
```

Run post-install checks only once you've decided whether they should
contact a running service:

```bash
onefite test --post-install --systemd-daemon
```

## Package notes

The installer provisions compilers, Grace, ImageMagick, TeX/PDF utilities,
HDF5 tools, FFmpeg, and other dependencies. Exact package availability
varies by Debian/Ubuntu release. Use the [Docker sandbox](docker.md) as a
reproducible diagnostic environment when host packages conflict.
