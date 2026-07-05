#!/usr/bin/env bash
#
# install-OFE-in-Arch.sh
#
# Arch Linux installer helper for OneFit-Engine.
#
# Uses Arch packages for system dependencies.
# Installs Rakubrew in the normal user account and uses it to install
# Rakudo and zef for that user.
# Creates root-owned /usr/local/bin links to the user-installed commands.
# Builds Grace/xmgrace from source when it is not already installed.
#
# Installs OFE as the normal sudo user, not as root.

set -u

if [[ "${EUID}" -ne 0 ]]; then
    echo "Run as root:"
    echo "  sudo $0 [install|uninstall]"
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

OS_ID="unknown"
OS_ID_LIKE=""
OS_VERSION_ID="unknown"

pkg_installed() {
    pacman -Q "$1" >/dev/null 2>&1
}

package_exists() {
    local pkg="$1"
    pacman -Si "$pkg" >/dev/null 2>&1 || pacman -Q "$pkg" >/dev/null 2>&1
}

install_pkg() {
    local pkg="$1"

    if pkg_installed "$pkg"; then
        echo "✓ package installed: $pkg"
        return 0
    fi

    echo "Installing package: $pkg"
    pacman -S --needed --noconfirm "$pkg" || {
        warn "could not install package: $pkg"
        return 1
    }
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

detect_os() {
    if [[ -r /etc/os-release ]]; then
        # shellcheck disable=SC1091
        . /etc/os-release
        OS_ID="${ID:-unknown}"
        OS_ID_LIKE="${ID_LIKE:-}"
        OS_VERSION_ID="${VERSION_ID:-unknown}"
    fi

    if [[ "$OS_ID" != "arch" && " $OS_ID_LIKE " != *" arch "* ]]; then
        warn "This script is intended for Arch Linux. Detected: ${OS_ID} ${OS_VERSION_ID}; ID_LIKE='${OS_ID_LIKE}'"
    fi
}

setup_paths() {
    log "System PATH setup"

    mkdir -p /etc/profile.d /etc/sudoers.d

    cat >/etc/profile.d/ofe-path.sh <<'PROFILE_EOF'
# OneFit-Engine / Arch Linux tools
for d in \
    /usr/local/bin \
    /usr/bin

do
    if [ -d "$d" ]; then
        case ":$PATH:" in
            *:"$d":*) ;;
            *) PATH="$d:$PATH" ;;
        esac
    fi
done

export PATH
PROFILE_EOF

    cat >/etc/sudoers.d/90-ofe-secure-path <<'SUDOERS_EOF'
Defaults secure_path=/usr/local/bin:/usr/bin:/usr/local/sbin:/usr/sbin:/sbin:/bin
SUDOERS_EOF

    chmod 644 /etc/profile.d/ofe-path.sh
    chmod 440 /etc/sudoers.d/90-ofe-secure-path

    if command -v visudo >/dev/null 2>&1; then
        visudo -cf /etc/sudoers.d/90-ofe-secure-path || {
            warn "invalid sudoers PATH file; removing it"
            rm -f /etc/sudoers.d/90-ofe-secure-path
        }
    fi

    export PATH="/usr/local/bin:/usr/bin:$PATH"
}

install_repositories() {
    log "Repositories / package database"
    pacman -Sy --noconfirm || die "pacman package database refresh failed"
}

install_packages() {
    log "Arch packages"

    # base-devel is a package group. It provides make, gcc and common build tools.
    pacman -S --needed --noconfirm base-devel || warn "could not install base-devel group"

    local packages=(
        perl
        git
        openssh
        apache
        swig
        gcc-fortran
        make
        tar
        wget
        curl
        jq
        zip
        unzip
        sudo
        vim
        man-db
        gnuplot
        openssl
        hdf5
        poppler
        ghostscript
        imagemagick
        jre-openjdk-headless
        ffmpeg
        which
		vim
		less
		apache
    )
	systemctl enable httpd
    systemctl start http

    local pkg
    for pkg in "${packages[@]}"; do
        install_pkg "$pkg"
    done

    # Package names that may vary or may not be in the enabled repositories.
    install_first_available_pkg perl-cgi || true
    install_first_available_pkg pdftk pdftk-java || true
    install_first_available_pkg texlive-bin texlive-basic || true
    install_first_available_pkg ufw firewalld || true
}


install_grace_from_source() {
    log "Install Grace/xmgrace from source"

    setup_paths

    if command -v xmgrace >/dev/null 2>&1; then
        echo "✓ xmgrace already found: $(command -v xmgrace)"
        link_cmd xmgrace
        link_cmd gracebat
        link_cmd grace
        return 0
    fi

    # Dependencies for building Grace with the X/Motif interface.
    # The Motif package name may differ according to repositories/AUR availability.
    local build_packages=(
        base-devel
		openmotif
		texlive
		linuxdoc-tools
        git
        gcc
        make
        autoconf
        automake
        libtool
        pkgconf
        xorgproto
        libx11
        libxext
        libxmu
        libxt
        libxpm
        freetype2
        libpng
        libjpeg-turbo
        zlib
    )

    local pkg
    for pkg in "${build_packages[@]}"; do
        install_pkg "$pkg"
    done

    install_first_available_pkg openmotif motif lesstif || \
        die "No Motif development package found. Install openmotif/motif/lesstif, then rerun this script."

    local tmpdir jobs oldpwd rc
    tmpdir="$(mktemp -d)"
    oldpwd="$PWD"
    jobs="$(nproc 2>/dev/null || echo 2)"

    echo "Building Grace in: $tmpdir"

    set +e
    git clone https://github.com/fxcoudert/xmgrace.git "$tmpdir/xmgrace"
    rc=$?
    set -e
    if [[ $rc -ne 0 ]]; then
        rm -rf "$tmpdir"
        die "could not clone Grace/xmgrace source"
    fi

    cd "$tmpdir/xmgrace" || {
        rm -rf "$tmpdir"
        die "could not enter Grace/xmgrace source directory"
    }

    chmod +x ./configure 2>/dev/null || true

    set +e
    ./configure CFLAGS="-std=gnu89 -Wno-error=implicit-function-declaration -Wno-error=int-conversion" --prefix=/usr/local
    rc=$?
    set -e
    if [[ $rc -ne 0 ]]; then
        cd "$oldpwd" || true
        rm -rf "$tmpdir"
        die "Grace configure failed"
    fi

    set +e
    make -j"$jobs"
    rc=$?
    set -e
    if [[ $rc -ne 0 ]]; then
        cd "$oldpwd" || true
        rm -rf "$tmpdir"
        die "Grace build failed"
    fi

    set +e
    make install
    rc=$?
    set -e
    cd "$oldpwd" || true
    rm -rf "$tmpdir"

    if [[ $rc -ne 0 ]]; then
        die "Grace installation failed"
    fi

    link_cmd xmgrace /usr/local/bin/xmgrace
    link_cmd gracebat /usr/local/bin/gracebat
    link_cmd xmgrace /usr/local/bin/grace

    echo "✓ Grace/xmgrace installed from source"
}

normal_user() {
    local u="${SUDO_USER:-$USER}"

    if [[ "$u" == "root" || -z "$u" ]]; then
        die "run this script with sudo from a normal user, not as root directly"
    fi

    echo "$u"
}

user_home() {
    local u="$1"
    getent passwd "$u" | cut -d: -f6
}

install_rakubrew() {
    log "Install Rakubrew system-wide"

    setup_paths

    mkdir -p /opt

    if [[ ! -x /opt/rakubrew/bin/rakubrew ]]; then
        rm -rf /opt/rakubrew

        # Official installer. RAKUBREW_HOME keeps the installation system-wide.
        (set -o pipefail; curl -fsSL https://rakubrew.org/install-on-perl.sh | \
            RAKUBREW_HOME=/opt/rakubrew sh) || die "Rakubrew installation failed"
    else
        echo "✓ Rakubrew already installed: /opt/rakubrew/bin/rakubrew"
    fi

    cat >/etc/profile.d/rakubrew.sh <<'PROFILE_EOF'
# System-wide Rakubrew for OneFit-Engine
export RAKUBREW_HOME=/opt/rakubrew
export PATH=/opt/rakubrew/bin:/opt/rakubrew/shims:/usr/local/bin:/usr/bin:$PATH
PROFILE_EOF

    chmod 644 /etc/profile.d/rakubrew.sh

    ln -sf /opt/rakubrew/bin/rakubrew /usr/local/bin/rakubrew

    export RAKUBREW_HOME=/opt/rakubrew
    export PATH=/opt/rakubrew/bin:/opt/rakubrew/shims:/usr/local/bin:/usr/bin:$PATH

    rakubrew mode shim || true

    echo 'Defaults env_keep += "RAKUBREW_HOME"' | sudo tee /etc/sudoers.d/91-rakubrew-env >/dev/null
    sudo chmod 440 /etc/sudoers.d/91-rakubrew-env

    if command -v visudo >/dev/null 2>&1; then
        sudo visudo -cf /etc/sudoers.d/91-rakubrew-env || {
            warn "invalid sudoers RAKUBREW_HOME file; removing it"
            sudo rm -f /etc/sudoers.d/91-rakubrew-env
        }
    fi
}

install_rakudo_and_zef() {
    log "Install Rakudo and zef system-wide (shared)"

    local shared_home="/opt/rakubrew"
    mkdir -p "$shared_home"

    export RAKUBREW_HOME="$shared_home"
    export PATH="$RAKUBREW_HOME/bin:$RAKUBREW_HOME/shims:/usr/local/bin:/usr/bin:$PATH"

    # Install rakubrew itself into the shared location if not already there
    if [[ ! -x "$RAKUBREW_HOME/bin/rakubrew" ]]; then
        mkdir -p "$RAKUBREW_HOME/bin"
        curl -fsSL https://rakubrew.org/install.sh -o /tmp/rakubrew-install.sh
        RAKUBREW_HOME="$shared_home" bash /tmp/rakubrew-install.sh
        rm -f /tmp/rakubrew-install.sh
    fi

    if ! rakubrew versions 2>/dev/null | grep -q "moar-"; then
        rakubrew download || rakubrew build moar
    else
        echo "✓ Rakudo already installed under system Rakubrew"
    fi

    version="$(rakubrew versions 2>/dev/null | sed "s/^* *//" | awk "/moar-/ {print \$1}" | sort -V | tail -n 1)"
    if [[ -n "$version" ]]; then
        rakubrew switch "$version"
    fi

    rakubrew build-zef

    # Make everything readable/executable by all users on the system
    chmod -R a+rX "$shared_home"

    echo "✓ raku: $(rakubrew which raku 2>/dev/null || command -v raku || true)"
    raku -v || true
    echo "✓ zef: $(rakubrew which zef 2>/dev/null || command -v zef || true)"
    zef --version || true

    # System-wide PATH setup so every user's shell picks up rakubrew's shims
    cat >/etc/profile.d/rakubrew.sh <<PROFILE_EOF
export RAKUBREW_HOME="$shared_home"
export PATH="\$RAKUBREW_HOME/shims:\$RAKUBREW_HOME/bin:\$PATH"
PROFILE_EOF
    chmod 644 /etc/profile.d/rakubrew.sh

    link_cmd rakubrew "$shared_home/bin/rakubrew"
    link_cmd raku      "$shared_home/shims/raku"
    link_cmd rakudo    "$shared_home/shims/rakudo"
    link_cmd zef       "$shared_home/shims/zef"
}

install_raku_modules() {
    log "Raku modules"

    if ! command -v zef >/dev/null 2>&1; then
        echo "WARNING: zef is not available; skipping Raku module installation" >&2
        exit 0
    fi

    zef update || true

    for mod in App::Prove6 Inline::Perl5 Readline Cro::HTTP::Router Pod::To::Man; do
        echo "Installing Raku module for user: $mod"
        zef install --/test "$mod" || echo "WARNING: could not install Raku module: $mod" >&2
    done

    #link_cmd prove6 "$ofe_home/.rakubrew/shims/prove6"
}

create_links() {
    log "Create /usr/local/bin links"

    local ofe_user ofe_home
    ofe_user="$(normal_user)"
    ofe_home="$(user_home "$ofe_user")"

    local cmds=(
        "rakubrew:$ofe_home/.rakubrew/bin/rakubrew"
        "raku:$ofe_home/.rakubrew/shims/raku"
        "rakudo:$ofe_home/.rakubrew/shims/rakudo"
        "zef:$ofe_home/.rakubrew/shims/zef"
        "prove6:$ofe_home/.rakubrew/shims/prove6"
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

    if command -v ufw >/dev/null 2>&1; then
        if ufw status 2>/dev/null | grep -q "Status: active"; then
            ufw allow 8142/tcp || warn "could not open port 8142 with ufw"
            ufw status || true
        else
            echo "ufw is installed but not active; not enabling it automatically"
        fi
        return 0
    fi

    if command -v firewall-cmd >/dev/null 2>&1; then
        systemctl enable --now firewalld >/dev/null 2>&1 || true
        if systemctl is-active --quiet firewalld; then
            firewall-cmd --permanent --add-port=8142/tcp || warn "could not open port 8142"
            firewall-cmd --reload || warn "could not reload firewalld"
            firewall-cmd --list-ports || true
        else
            warn "firewalld is not active; skipping firewall configuration"
        fi
        return 0
    fi

    warn "no supported firewall tool found; skipping firewall configuration"
}

ofe_install() {
    log "Install OFE as normal user"

    OFE_USER="${SUDO_USER:-$USER}"

    if [[ "$OFE_USER" == "root" ]]; then
        die "run this script with sudo from a normal user, not as root directly"
    fi

    sudo -u "$OFE_USER" -H bash -lc '
        set -euo pipefail

        export PATH=/usr/local/bin:/usr/bin:$PATH

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

        ./INSTALL --no-test 
    '
}

uninstall_ofe() {
    log "Uninstall OFE"

    OFE_USER="${SUDO_USER:-$USER}"

    if [[ "$OFE_USER" == "root" ]]; then
        warn "Cannot determine normal user; skipping user OFE/Rakubrew removal"
    else
        sudo -u "$OFE_USER" -H bash -lc '
            rm -rf "$HOME/.local/OneFit-Engine"
            rm -rf "$HOME/.rakubrew"
            rm -f  "$HOME/.local/bin/onefite"
            rm -f  "$HOME/.local/bin/ofe"
            rm -f  "$HOME/.local/bin/onefit"
            rm -f  "$HOME/.local/bin/of"
        '
    fi

    rm -f /usr/local/bin/onefite
    rm -f /usr/local/bin/ofe
    rm -f /usr/local/bin/onefit
    rm -f /usr/local/bin/of
    rm -f /usr/local/bin/rakubrew
    rm -f /usr/local/bin/raku
    rm -f /usr/local/bin/rakudo
    rm -f /usr/local/bin/zef
    rm -f /usr/local/bin/prove6
    rm -f /usr/local/bin/xmgrace
    rm -f /usr/local/bin/gracebat
    rm -f /usr/local/bin/grace

    rm -f /etc/profile.d/ofe-path.sh
    rm -f /etc/profile.d/rakubrew.sh
    rm -f /etc/sudoers.d/90-ofe-secure-path

    if command -v ufw >/dev/null 2>&1 && ufw status 2>/dev/null | grep -q "Status: active"; then
        ufw delete allow 8142/tcp >/dev/null 2>&1 || true
    fi

    if command -v firewall-cmd >/dev/null 2>&1; then
        firewall-cmd --permanent --remove-port=8142/tcp >/dev/null 2>&1 || true
        firewall-cmd --reload >/dev/null 2>&1 || true
    fi

    echo "OFE files and links removed."
    echo "Arch packages installed by this script were not removed."
}

final_checks() {
    log "Final checks"

    local cmds=(
        perl git sshd httpd swig gcc gfortran make wget curl jq zip unzip
        sudo vim man gnuplot h5dump pdftk epstopdf ffmpeg
        rakubrew raku rakudo zef prove6 xmgrace gracebat
    )

    local cmd
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

main() {
    case "${1:-install}" in
        install)
            detect_os
            setup_paths
            install_repositories
            install_packages
            install_rakubrew
            install_rakudo_and_zef
            install_raku_modules
            install_grace_from_source
            create_links
            open_ports
            ofe_install
            final_checks
            echo
            echo "Done."
            ;;
        uninstall|remove)
            detect_os
            uninstall_ofe
            ;;
        *)
            echo "Usage: sudo $0 [install|uninstall]"
            exit 1
            ;;
    esac
}

main "$@"
