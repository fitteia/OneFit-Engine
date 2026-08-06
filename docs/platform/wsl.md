# Windows Subsystem for Linux

WSL2 provides a convenient Linux environment for CLI use. Keep source and
fit work inside the Linux filesystem for better performance.

## Install WSL2

From an elevated PowerShell prompt:

```powershell
wsl --install -d Debian
wsl --set-default-version 2
```

Open the Debian shell and install prerequisites:

```bash
sudo apt-get update
sudo apt-get install -y git raku
```

## Install OneFit-Engine

```bash
mkdir -p "$HOME/.local" && cd "$HOME/.local"
git clone https://github.com/fitteia/OneFit-Engine.git && cd OneFit-Engine
./INSTALL --wsl
```

`--wsl` doesn't disable Debian package installation (unlike `--docker`), so
this single command provisions dependencies and installs OFE. It does
disable several ordinary site-install defaults, including the regular and
post-install test suites - run them explicitly afterward. If you'd rather
provision dependencies the same, more thoroughly-tested way as on bare
Debian, run `sudo ./pre-install-OFE-in-Linux.sh` first instead - see
[Debian and Ubuntu](debian-ubuntu.md).

## Validate CLI operation

```bash
onefite --version
onefite test --no-systemd-daemon --no-post-install
```

## Service access

For local Windows access, bind the service to loopback first:

```bash
onefite service start --ip=127.0.0.1
```

WSL networking behavior varies by Windows and WSL version. Don't change
the bind address merely to work around access problems - confirm firewall
and forwarding behavior first, then read
[service security](../security.md).

Modern WSL distributions may support systemd. If yours doesn't, use the
non-systemd background service, or run `start-web-engine` in a supervised
terminal.

## Filesystem guidance

Prefer paths under `$HOME` rather than `/mnt/c` for compilation and
I/O-intensive fitting - cross-filesystem access is significantly slower.
Export only the final JSON, ZIP, PDF, and data products to Windows-mounted
paths.
