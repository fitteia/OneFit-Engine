#!/usr/bin/env bash
#
# install-OFE-in-Linux.sh
#
# OneFit-Engine installer helper for Debian-like and Fedora/RHEL-like Linux.
#
# Supported families:
#   - Debian / Ubuntu / Linux Mint / Raspbian: apt + dpkg
#   - Fedora / CentOS Stream / Rocky / AlmaLinux / RHEL: dnf + rpm
#
# Raku/zef policy:
#   - Debian-like systems use Debian packages: rakudo + perl6-zef
#   - Fedora/RHEL-like systems use packages when available:
#       rakudo + perl6-zef/perl6-zef/zef if available
#     If zef is not packaged, Fedora/RHEL falls back to rakubrew build-zef.
#
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

OS_ID="unknown"
OS_ID_LIKE=""
OS_VERSION_ID="unknown"
OS_CODENAME="unknown"
OS_FAMILY="unknown"
PM="unknown"
WEB_SERVER_CMD="unknown"

is_debian_like() {
    [[ "$OS_ID" =~ ^(debian|ubuntu|linuxmint|raspbian)$ || " $OS_ID_LIKE " == *" debian "* ]]
}

is_fedora_like() {
    [[ "$OS_ID" =~ ^(fedora|centos|centos-stream|rocky|almalinux|rhel|redhat)$ || " $OS_ID_LIKE " == *" rhel "* || " $OS_ID_LIKE " == *" fedora "* ]]
}

detect_os() {
    if [[ -r /etc/os-release ]]; then
        # shellcheck disable=SC1091
        . /etc/os-release
        OS_ID="${ID:-unknown}"
        OS_ID_LIKE="${ID_LIKE:-}"
        OS_VERSION_ID="${VERSION_ID:-unknown}"
        OS_CODENAME="${VERSION_CODENAME:-unknown}"
    fi

    if is_debian_like; then
        OS_FAMILY="debian"
        PM="apt"
        WEB_SERVER_CMD="apache2"
    elif is_fedora_like; then
        OS_FAMILY="fedora"
        PM="dnf"
        WEB_SERVER_CMD="httpd"
    else
        die "unsupported Linux distribution: ${OS_ID} ${OS_VERSION_ID}; ID_LIKE='${OS_ID_LIKE}'"
    fi

    log "Detected OS"
    echo "ID=${OS_ID} VERSION_ID=${OS_VERSION_ID} ID_LIKE=${OS_ID_LIKE:-none}"
    echo "Using package family: ${OS_FAMILY}"
}

pkg_installed() {
    local pkg="$1"

    case "$PM" in
        apt)
            dpkg-query -W -f='${Status}' "$pkg" 2>/dev/null | grep -q "install ok installed"
            ;;
        dnf)
            rpm -q "$pkg" >/dev/null 2>&1
            ;;
    esac
}

package_exists() {
    local pkg="$1"

    case "$PM" in
        apt)
            apt-cache show "$pkg" >/dev/null 2>&1 || dpkg-query -W "$pkg" >/dev/null 2>&1
            ;;
        dnf)
            dnf -q list --available "$pkg" >/dev/null 2>&1 || dnf -q list --installed "$pkg" >/dev/null 2>&1
            ;;
    esac
}

install_pkg() {
    local pkg="$1"

    if pkg_installed "$pkg"; then
        echo "✓ package installed: $pkg"
        return 0
    fi

    echo "Installing package: $pkg"
    case "$PM" in
        apt)
            DEBIAN_FRONTEND=noninteractive apt-get install -y "$pkg" || {
                warn "could not install package: $pkg"
                return 1
            }
            ;;
        dnf)
            dnf install -y "$pkg" || {
                warn "could not install package: $pkg"
                return 1
            }
            ;;
    esac
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
                /opt/rakubrew \
                /opt \
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

    if [[ "$OS_FAMILY" == "fedora" ]]; then
        cat >/etc/profile.d/ofe-path.sh <<'PROFILE_EOF'
# OneFit-Engine / Fedora-RHEL / Raku tools
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
PROFILE_EOF
        cat >/etc/sudoers.d/90-ofe-secure-path <<'SUDOERS_EOF'
Defaults secure_path=/usr/local/bin:/usr/bin:/opt/rakubrew/bin:/opt/rakubrew/shims:/opt/rakubrew/versions/fedora/bin:/usr/local/sbin:/usr/sbin:/sbin:/bin
SUDOERS_EOF
        export RAKUBREW_HOME=/opt/rakubrew
        export PATH="/usr/local/bin:/usr/bin:/opt/rakubrew/bin:/opt/rakubrew/shims:/opt/rakubrew/versions/fedora/bin:$PATH"
    else
        cat >/etc/profile.d/ofe-path.sh <<'PROFILE_EOF'
# OneFit-Engine / Debian tools
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
        export PATH="/usr/local/bin:/usr/bin:$PATH"
    fi

    chmod 644 /etc/profile.d/ofe-path.sh
    chmod 440 /etc/sudoers.d/90-ofe-secure-path

    if command -v visudo >/dev/null 2>&1; then
        visudo -cf /etc/sudoers.d/90-ofe-secure-path || {
            warn "invalid sudoers PATH file; removing it"
            rm -f /etc/sudoers.d/90-ofe-secure-path
        }
    fi
}

install_repositories() {
    log "Repositories"

    if [[ "$OS_FAMILY" == "debian" ]]; then
        apt-get update || die "apt-get update failed"
        install_pkg ca-certificates
        install_pkg apt-transport-https || true
        return 0
    fi

    install_pkg dnf-plugins-core || true

    if [[ "$OS_ID" == "fedora" ]] && ! rpm -q rpmfusion-free-release >/dev/null 2>&1; then
        local fedora_ver
        fedora_ver="$(rpm -E %fedora 2>/dev/null || true)"

        if [[ -n "$fedora_ver" && "$fedora_ver" != "%fedora" ]]; then
            dnf install -y \
                "https://download1.rpmfusion.org/free/fedora/rpmfusion-free-release-${fedora_ver}.noarch.rpm" \
                || warn "could not install RPM Fusion free release"
        else
            warn "could not determine Fedora version for RPM Fusion"
        fi
    elif rpm -q rpmfusion-free-release >/dev/null 2>&1; then
        echo "✓ package installed: rpmfusion-free-release"
    else
        warn "RPM Fusion auto-install is only attempted for Fedora; ffmpeg may be unavailable on ${OS_ID}"
    fi

    dnf makecache || true
}

install_packages_debian() {
    log "Debian packages"

    local packages=(
        perl
        libperl-dev
        libcgi-pm-perl
        git
        openssh-server
        apache2
        swig
        gcc
        gfortran
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
        libssl-dev
        hdf5-tools
        libhdf5-dev
        poppler-utils
        texlive-font-utils
        ghostscript
        imagemagick
        default-jre-headless
        rakudo
        perl6-zef
        grace
    )

    local pkg
    for pkg in "${packages[@]}"; do
        install_pkg "$pkg"
    done

    install_first_available_pkg pdftk-java pdftk || true
    install_first_available_pkg ffmpeg || true
    install_first_available_pkg ufw firewalld || true
}

install_packages_fedora() {
    log "Fedora/RHEL packages"

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
        ghostscript
        ImageMagick
        java-21-openjdk
        firewalld
        rakudo
        xmgrace
    )

    local pkg
    for pkg in "${packages[@]}"; do
        install_pkg "$pkg"
    done

    install_first_available_pkg texlive-epstopdf texlive-epstopdf-bin || true
    install_first_available_pkg pdftk-java pdftk || true
    install_first_available_pkg ffmpeg || true

    # Use distribution zef package if one exists.
    install_first_available_pkg perl6-zef raku-zef zef || true
}

install_packages() {
    if [[ "$OS_FAMILY" == "debian" ]]; then
        install_packages_debian
    else
        install_packages_fedora
    fi
}

setup_raku_and_zef_debian() {
    log "Debian Raku / zef"

    setup_paths

    if ! command -v raku >/dev/null 2>&1; then
        die "Debian rakudo package did not provide raku"
    fi

    if ! command -v zef >/dev/null 2>&1; then
        die "Debian perl6-zef package did not provide zef"
    fi

    echo "✓ raku found: $(command -v raku)"
    raku -v || true
    echo "✓ zef found: $(command -v zef)"
    zef --version || true

    link_cmd raku /usr/bin/raku
    link_cmd rakudo /usr/bin/rakudo
    link_cmd zef /usr/bin/zef
}

setup_raku_and_zef_fedora() {
    log "Fedora/RHEL Raku / zef"

    setup_paths

    if ! command -v raku >/dev/null 2>&1; then
        die "Fedora/RHEL rakudo package did not provide raku"
    fi

    echo "✓ raku found: $(command -v raku)"
    raku -v || true

    if command -v zef >/dev/null 2>&1 && zef --version >/dev/null 2>&1; then
        echo "✓ zef found: $(command -v zef)"
        zef --version || true
    else
        warn "zef package not available/working; falling back to rakubrew build-zef"

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
        rakubrew build-zef || warn "rakubrew build-zef failed"

        link_cmd rakubrew /usr/local/bin/rakubrew
    fi

    link_cmd raku /usr/bin/raku
    link_cmd rakudo /usr/bin/rakudo
    link_cmd zef

    chmod -R a+rX /opt/rakubrew 2>/dev/null || true
}

install_raku_modules() {
    log "Raku modules"

    if ! command -v zef >/dev/null 2>&1; then
        warn "zef is not available; skipping Raku module installation"
        return 0
    fi

    zef update || true

    local mod
    for mod in App::Prove6 Inline::Perl5 Readline Cro::HTTP::Router Pod::To::Man; do
        echo "Installing Raku module: $mod"
        zef install --/test "$mod" || warn "could not install Raku module: $mod"
    done

    link_cmd prove6
}

setup_raku_and_zef() {
    if [[ "$OS_FAMILY" == "debian" ]]; then
        setup_raku_and_zef_debian
    else
        setup_raku_and_zef_fedora
    fi

    install_raku_modules
}

create_links() {
    log "Create /usr/local/bin links"

    local cmds=(
        raku:/usr/bin/raku
        rakudo:/usr/bin/rakudo
        zef
        prove6
        xmgrace
        gracebat
        grace:/usr/bin/xmgrace
    )

    if [[ "$OS_FAMILY" == "fedora" ]]; then
        cmds+=(rakubrew:/usr/local/bin/rakubrew)
    fi

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

    local webcmd="$WEB_SERVER_CMD"
    local cmds=(
        perl git sshd "$webcmd" swig gcc gfortran make wget curl jq zip unzip
        sudo vim man gnuplot h5dump pdftk epstopdf ffmpeg
        raku rakudo zef prove6 xmgrace gracebat
    )

    if [[ "$OS_FAMILY" == "fedora" ]]; then
        cmds+=(rakubrew)
    fi

    local cmd
    for cmd in "${cmds[@]}"; do
        if [[ -z "$cmd" ]]; then
            continue
        fi
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
    detect_os
    setup_paths
    install_repositories
    install_packages
    setup_raku_and_zef
    create_links
    open_ports
    ofe_install
    final_checks

    echo
    echo "Done."
}

main "$@"
