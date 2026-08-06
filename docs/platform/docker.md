# Docker and container test environments

The repository supplies per-distribution Dockerfiles under `Dockerfiles/`
for Debian, Ubuntu, Fedora, Arch, openSUSE, CentOS Stream, and AlmaLinux.
`test-ofe-in-docker-sandbox.sh` builds and runs them consistently. (The
repository root also has a single standalone `Dockerfile`, a Debian-trixie
image with the full dependency set pre-baked, used by the macOS
Colima/Docker path in [macOS](macos.md) - the two are independent.)

## Requirements

Install Docker. On macOS, Colima is one option:

```bash
brew install colima docker docker-buildx
colima start --memory 4 --runtime docker
```

## Build an image

From the repository root:

```bash
./test-ofe-in-docker-sandbox.sh build debian
```

Build all defined images:

```bash
./test-ofe-in-docker-sandbox.sh build all
```

Available names: `debian`, `ubuntu`, `fedora`, `arch`, `suse`, `centos`,
`alma`.

## Run a container

```bash
HOST_WORK="$PWD/work" PORT=8142 \
  ./test-ofe-in-docker-sandbox.sh run debian
```

Layout:

```text
/home/ofe/.local   private container installation area
/home/ofe/work     host-mounted work directory
8142               HTTP service port
```

Containers are removed on exit by default. Set `KEEP=1` to retain one,
then open another shell with:

```bash
./test-ofe-in-docker-sandbox.sh shell debian
```

## Install inside the container

**The per-distro images are a base development environment, not a
complete OneFit-Engine dependency set** - for example, the Debian and
Ubuntu images install `raku` itself but not `zef`, `swig`, or
`libcgi-pm-perl`, all of which OFE needs. Don't skip straight to
`./INSTALL --docker` (or bare `./INSTALL`) expecting it to provision the
rest: OFE's own container detection sets `--docker` automatically whenever
it's run inside *any* container, and `--docker` disables `./INSTALL`'s own
package-installation step (`--dpkg` becomes `False`) - so on these minimal
images, nothing will install the missing pieces for you.

Run the platform pre-install script first, exactly as you would on a bare
host:

```bash
cd /home/ofe/.local
git clone https://github.com/fitteia/OneFit-Engine.git && cd OneFit-Engine
sudo ./pre-install-OFE-in-Linux.sh
./INSTALL --no-test --no-post-test && onefite service start && ./post-install.sh
```

Validate with CLI tests appropriate to the image:

```bash
onefite --version
onefite test --no-systemd-daemon --no-post-install
```

## Service access

The helper publishes host `PORT` to container port 8142. A process inside
the container must listen on `0.0.0.0:8142` to be reachable through that
mapping (`onefite service start`'s default `--ip=0.0.0.0` already does
this). Published ports make the unauthenticated service accessible
according to the Docker host's network rules - read
[security](../security.md) before publishing it beyond a private
development machine.

## Support interpretation

A Dockerfile existing for a distribution indicates an intended test
environment, not a guarantee that every scientific dependency is available
in every distribution's repositories. Record actual build/test results
when declaring platform support, rather than inferring it from the
Dockerfile's existence.
