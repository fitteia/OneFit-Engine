# Other Linux distributions

`pre-install-OFE-in-Linux.sh` includes package-manager support for APT,
DNF, Pacman, and Zypper - it targets Fedora/RHEL derivatives, Arch-family
systems, and openSUSE, in addition to Debian-family systems (see
[Debian and Ubuntu](debian-ubuntu.md) for that path).

## Preparation

Install Git, then run the helper after reviewing it:

```bash
git clone https://github.com/fitteia/OneFit-Engine.git && cd OneFit-Engine
sudo ./pre-install-OFE-in-Linux.sh
```

Then install. `./INSTALL` auto-detects Fedora/CentOS/Arch/openSUSE and
already disables its own (Debian-only) package step on those systems, so
`--no-dpkg` below is a defensive, explicit way to state the same thing:

```bash
./INSTALL --no-dpkg --no-web-server
```

For a user-space installation, also add `--to-user --no-enable-gs`.

## Distribution families

### Fedora, RHEL, CentOS, Alma, Rocky

The helper uses DNF, and may enable CRB/PowerTools, EPEL, or RPM Fusion to
find scientific and multimedia packages. Review repository changes before
use.

### Arch and derivatives

The helper uses Pacman, and may install `base-devel`. Package names and
Raku availability can differ from Debian-family documentation.

### openSUSE

The helper uses Zypper, and disables ImageMagick policy modification in
the main installer path. Verify Grace, Raku, HDF5, and PDF tool
availability.

## Validate incrementally

```bash
command -v raku zef gcc gfortran
onefite --version
onefite test --no-systemd-daemon --no-post-install
```

Then test plotting, HDF5 import, and the service separately - a successful
CLI fit doesn't prove the optional PDF, video, HDF5, or HTTP features work.

Use the corresponding [Docker sandbox](docker.md) image to compare package
behavior when a host installation fails.
