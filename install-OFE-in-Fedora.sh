#!/usr/bin/env bash
#
# install-OFE-in-Fedora.sh
#
# Fedora Server installer helper for OneFit-Engine.
#
# Uses Fedora packages for:
#   - rakudo
#   - xmgrace
#
# Uses rakubrew to install zef system-wide.
# Installs OFE as the normal sudo user, not as root.

set -u

if [[ "${EUID}" -ne 0 ]]; then
    echo "Run as root:"
    echo "  sudo $0"
    exit 1
fi

log() {
    echo
    echo "=== $* ==="
}

warn() {
    echo "WARNING: $*" >&2
}

die() {
    echo "ERROR: $*" >&2
    exit 1
}

detect_os() {
    if [[ -r /etc/os-release ]]; then
        # shellcheck disable=SC1091
        . /etc/os-release
        OS_ID="${ID:-unknown}"
        OS_VERSION_ID="${VERSION_ID:-unknown}"
    else
        OS_ID="unknown"
        OS_VERSION_ID="unknown"
    fi
}

install_pkg() {
    local pkg="$1"

    if rpm -q "$pkg" >/dev/null 2>&1; then
        echo "✓ package installed: $pkg"
        return 0
    fi

    echo "Installing package: $pkg"
    dnf install -y "$pkg" || {
        warn "could not install package: $pkg"
        return 1
    }
}

package_exists() {
    local pkg="$1"
    dnf -q list --available "$pkg" >/dev/null 2>&1 || \
    dnf -q list --installed "$pkg" >/dev/null 2>&1
}

install_first_available_pkg() {
    local pkg

    for pkg in "$@"; do
        if package_exists "$pkg"; then
            install_pkg "$pkg"
            return 0
        fi
    done

    warn "none of these packages are available: $*"
    return 1
}

link_cmd() {
    local cmd="$1"
    local explicit_path="${2:-}"
    local target=""

    mkdir -p /usr/local/bin

    if [[ -n "$explicit_path" && -x "$explicit_path" ]]; then
        target="$explicit_path"
    else
        target="$(
            find \
                /opt/rakubrew \
                /opt \
                /usr \
                /usr/local \
                -type f \
                -name "$cmd" \
                -perm -111 \
                ! -path "/usr/local/bin/$cmd" \
                2>/dev/null | head -n 1
        )"
    fi

    if [[ -n "$target" && -x "$target" ]]; then
        ln -sf "$target" "/usr/local/bin/$cmd"
        chmod 755 "$target" 2>/dev/null || true
        echo "✓ /usr/local/bin/$cmd -> $target"
    elif command -v "$cmd" >/dev/null 2>&1; then
        echo "✓ $cmd already in PATH: $(command -v "$cmd")"
    else
        warn "$cmd not found; no link created"
    fi
}

setup_paths() {
    log "System PATH setup"

    mkdir -p /etc/profile.d
    cat >/etc/profile.d/ofe-path.sh <<'EOF'
# OneFit-Engine / Fedora / Rakubrew tools
export RAKUBREW_HOME=/opt/rakubrew

for d in \
    /usr/local/bin \
    /usr/bin \
    /opt/rakubrew/bin \
    /opt/rakubrew/shims \
    /opt/rakubrew/versions/fedora/bin
do
    if [ -d "$d" ]; then
        case ":$PATH:" in
            *:"$d":*) ;;
            *) PATH="$d:$PATH" ;;
        esac
    fi
done

export PATH
EOF
    chmod 644 /etc/profile.d/ofe-path.sh

    mkdir -p /etc/sudoers.d
    cat >/etc/sudoers.d/90-ofe-secure-path <<'EOF'
Defaults secure_path=/usr/local/bin:/usr/bin:/opt/rakubrew/bin:/opt/rakubrew/shims:/opt/rakubrew/versions/fedora/bin:/usr/local/sbin:/usr/sbin:/sbin:/bin
EOF
    chmod 440 /etc/sudoers.d/90-ofe-secure-path

    if command -v visudo >/dev/null 2>&1; then
        visudo -cf /etc/sudoers.d/90-ofe-secure-path || {
            warn "invalid sudoers PATH file; removing it"
            rm -f /etc/sudoers.d/90-ofe-secure-path
        }
    fi

    export RAKUBREW_HOME=/opt/rakubrew
    export PATH="/usr/local/bin:/usr/bin:/opt/rakubrew/bin:/opt/rakubrew/shims:/opt/rakubrew/versions/fedora/bin:$PATH"
}

install_repositories() {
    log "Repositories"

    install_pkg dnf-plugins-core

    if ! rpm -q rpmfusion-free-release >/dev/null 2>&1; then
        local fedora_ver
        fedora_ver="$(rpm -E %fedora 2>/dev/null || true)"

        if [[ -n "$fedora_ver" && "$fedora_ver" != "%fedora" ]]; then
            dnf install -y \
                "https://download1.rpmfusion.org/free/fedora/rpmfusion-free-release-${fedora_ver}.noarch.rpm" \
                || warn "could not install RPM Fusion free release"
        else
            warn "could not determine Fedora version for RPM Fusion"
        fi
    else
        echo "✓ package installed: rpmfusion-free-release"
    fi

    dnf makecache || true
}

install_packages() {
    log "Fedora packages"

    local packages=(
        perl
        perl-devel
        perl-CGI
        git
        openssh-server
        httpd
        swig
        gcc
        gcc-gfortran
        make
        tar
        wget
        curl
        jq
        zip
        unzip
        sudo
        vim-enhanced
        man-db
        gnuplot
        openssl-devel
        hdf5
        hdf5-devel
        poppler-utils
        texlive-epstopdf
        ghostscript
        ImageMagick
        java-21-openjdk
        firewalld

        # Fedora packages requested by user
        rakudo
        xmgrace
    )

    for pkg in "${packages[@]}"; do
        install_pkg "$pkg"
    done

    install_first_available_pkg pdftk-java pdftk || true
    install_first_available_pkg ffmpeg-free ffmpeg || true
}

install_rakubrew_and_zef() {
    log "Rakubrew / system-wide zef"

    setup_paths

    if ! command -v raku >/dev/null 2>&1; then
        die "Fedora rakudo package did not provide raku"
    fi

    echo "✓ raku found: $(command -v raku)"
    raku -v || true

    mkdir -p /opt/rakubrew/bin /opt/rakubrew/shims /opt/rakubrew/versions

    if [[ ! -x /usr/local/bin/rakubrew ]]; then
        echo "Installing rakubrew to /usr/local/bin/rakubrew"
        curl -fsSL https://rakubrew.org/perl/rakubrew \
            -o /usr/local/bin/rakubrew
        chmod 755 /usr/local/bin/rakubrew
    else
        echo "✓ rakubrew found: /usr/local/bin/rakubrew"
    fi

    export RAKUBREW_HOME=/opt/rakubrew
    export PATH="/usr/local/bin:/usr/bin:/opt/rakubrew/bin:/opt/rakubrew/shims:$PATH"

    rakubrew register fedora /usr 2>/dev/null || true
    rakubrew switch fedora 2>/dev/null || true
    rakubrew mode shim 2>/dev/null || true

    if ! command -v zef >/dev/null 2>&1 || ! zef --version >/dev/null 2>&1; then
        echo "Installing zef with rakubrew build-zef"
        rakubrew build-zef || warn "rakubrew build-zef failed"
    else
        echo "✓ zef already works: $(command -v zef)"
    fi

    link_cmd rakubrew /usr/local/bin/rakubrew
    link_cmd raku /usr/bin/raku
    link_cmd rakudo /usr/bin/rakudo
    link_cmd zef
    link_cmd prove6

    chmod -R a+rX /opt/rakubrew 2>/dev/null || true

    if ! command -v zef >/dev/null 2>&1; then
        warn "zef is still not in PATH; trying to locate it"
        find /opt/rakubrew /usr/local /usr -name zef -type f -perm -111 2>/dev/null || true
    fi

    if command -v zef >/dev/null 2>&1; then
        zef update || true

        for mod in App::Prove6 Inline::Perl5 Readline Cro::HTTP::Router Pod::To::Man; do
            echo "Installing Raku module: $mod"
            zef install --/test "$mod" || warn "could not install Raku module: $mod"
        done
    else
        warn "zef still not available; skipping Raku module installation"
    fi

    link_cmd prove6
}

create_links() {
    log "Create /usr/local/bin links"

    local cmds=(
        raku:/usr/bin/raku
        rakudo:/usr/bin/rakudo
        rakubrew:/usr/local/bin/rakubrew
        zef
        prove6
        xmgrace
        gracebat
        grace
    )

    local item cmd path
    for item in "${cmds[@]}"; do
        cmd="${item%%:*}"
        if [[ "$item" == *:* ]]; then
            path="${item#*:}"
        else
            path=""
        fi
        link_cmd "$cmd" "$path"
    done
}

open_ports() {
    log "Open port 8142 for OneFit-Engine"

    if ! command -v firewall-cmd >/dev/null 2>&1; then
        warn "firewall-cmd not found; skipping firewall configuration"
        return 0
    fi

    systemctl enable --now firewalld >/dev/null 2>&1 || true

    if systemctl is-active --quiet firewalld; then
        firewall-cmd --permanent --add-port=8142/tcp || warn "could not open port 8142"
        firewall-cmd --reload || warn "could not reload firewalld"
        firewall-cmd --list-ports || true
    else
        warn "firewalld is not active; skipping firewall configuration"
    fi
}

ofe_install() {
    log "Install OFE as normal user"

    OFE_USER="${SUDO_USER:-$USER}"

    if [[ "$OFE_USER" == "root" ]]; then
        die "run this script with sudo from a normal user, not as root directly"
    fi

    sudo -u "$OFE_USER" -H bash -lc '
        set -euo pipefail

        export RAKUBREW_HOME=/opt/rakubrew
        export PATH=/usr/local/bin:/usr/bin:/opt/rakubrew/bin:/opt/rakubrew/shims:$PATH

        mkdir -p "$HOME/public_html"
        mkdir -p "$HOME/.local"

        if [[ ! -d "$HOME/.local/OneFit-Engine/.git" ]]; then
            rm -rf "$HOME/.local/OneFit-Engine"
            git clone https://github.com/fitteia/OneFit-Engine.git \
                "$HOME/.local/OneFit-Engine"
        else
            cd "$HOME/.local/OneFit-Engine"
            git pull --ff-only || true
        fi

        cd "$HOME/.local/OneFit-Engine"

        chmod +x ./INSTALL 2>/dev/null || true

        ./INSTALL -d
    '
}

final_checks() {
    log "Final checks"

    local cmds=(
        perl git sshd httpd swig gcc gfortran make wget curl jq zip unzip
        sudo vim man gnuplot h5dump pdftk epstopdf ffmpeg
        raku rakudo rakubrew zef prove6 xmgrace gracebat
    )

    for cmd in "${cmds[@]}"; do
        if command -v "$cmd" >/dev/null 2>&1; then
            printf "✓ %-10s %s\n" "$cmd" "$(command -v "$cmd")"
        else
            printf "✗ %-10s not found\n" "$cmd"
        fi
    done

    echo
    echo "Normal users will get the PATH after a new login."
    echo "For the current shell, run:"
    echo "  source /etc/profile.d/ofe-path.sh"
}

detect_os

if [[ "$OS_ID" != "fedora" ]]; then
    warn "This script is intended for Fedora. Detected: ${OS_ID} ${OS_VERSION_ID}"
fi

setup_paths
install_repositories
install_packages
install_rakubrew_and_zef
create_links
open_ports
ofe_install
final_checks

echo
echo "Done."
