#!/usr/bin/env bash
#
# install-OFE-in-Linux.sh
#
# Single OneFit-Engine installer for:
#   - Debian / Ubuntu / Linux Mint / Raspberry Pi OS: apt
#   - Fedora / CentOS Stream / Rocky / Alma / RHEL: dnf
#   - Arch / Manjaro / EndeavourOS: pacman
#   - openSUSE Leap / Tumbleweed: zypper
#
# Run:
#   sudo ./install-OFE-in-Linux.sh install
#   sudo ./install-OFE-in-Linux.sh uninstall
#
# OFE is installed as the normal sudo user, not as root.

set -u

if [[ "${EUID}" -ne 0 ]]; then
    echo "Run as root:"
    echo "  sudo $0 [install|uninstall]"
    exit 1
fi

log()  { echo; echo "=== $* ==="; }
warn() { echo "WARNING: $*" >&2; }
die()  { echo "ERROR: $*" >&2; exit 1; }

OS_ID="unknown"
OS_ID_LIKE=""
OS_VERSION_ID="unknown"
OS_CODENAME="unknown"
OS_FAMILY="unknown"
PM="unknown"
WEB_SERVER_CMD="unknown"

is_arch_like() {
    [[ "$OS_ID" == "arch" || " $OS_ID_LIKE " == *" arch "* ]]
}

is_debian_like() {
    [[ "$OS_ID" =~ ^(debian|ubuntu|linuxmint|raspbian|raspberrypi)$ || " $OS_ID_LIKE " == *" debian "* ]]
}

is_fedora_like() {
    [[ "$OS_ID" =~ ^(fedora|centos|centos-stream|rocky|almalinux|rhel|redhat|ol)$ || \
       " $OS_ID_LIKE " == *" rhel "* || " $OS_ID_LIKE " == *" fedora "* ]]
}

is_suse_like() {
    [[ "$OS_ID" =~ ^(opensuse|opensuse-leap|opensuse-tumbleweed|sles|sled)$ || \
       " $OS_ID_LIKE " == *" suse "* ]]
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

    if is_arch_like; then
        OS_FAMILY="arch"
        PM="pacman"
        WEB_SERVER_CMD="httpd"
    elif is_debian_like; then
        OS_FAMILY="debian"
        PM="apt"
        WEB_SERVER_CMD="apache2"
    elif is_fedora_like; then
        OS_FAMILY="fedora"
        PM="dnf"
        WEB_SERVER_CMD="httpd"
    elif is_suse_like; then
        OS_FAMILY="suse"
        PM="zypper"
        WEB_SERVER_CMD="apache2"
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
        pacman) pacman -Q "$pkg" >/dev/null 2>&1 ;;
        apt)    dpkg-query -W -f='${Status}' "$pkg" 2>/dev/null | grep -q "install ok installed" ;;
        dnf)    rpm -q "$pkg" >/dev/null 2>&1 ;;
        zypper) rpm -q "$pkg" >/dev/null 2>&1 ;;
    esac
}

package_exists() {
    local pkg="$1"
    case "$PM" in
        pacman) pacman -Si "$pkg" >/dev/null 2>&1 || pacman -Q "$pkg" >/dev/null 2>&1 ;;
        apt)    apt-cache show "$pkg" >/dev/null 2>&1 || dpkg-query -W "$pkg" >/dev/null 2>&1 ;;
        dnf)    dnf -q list --available "$pkg" >/dev/null 2>&1 || dnf -q list --installed "$pkg" >/dev/null 2>&1 ;;
        zypper)
            # Match a line whose second '|'-delimited column is exactly $pkg,
            # e.g. "i  | rakudo | package | ...". Tolerates leading status char.
            zypper --non-interactive search --exact-match "$pkg" 2>/dev/null \
                | awk -F'|' -v p="$pkg" '{gsub(/^[ \t]+|[ \t]+$/, "", $2); if ($2 == p) found=1} END{exit !found}' \
                || rpm -q "$pkg" >/dev/null 2>&1
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
        pacman)
            pacman -S --needed --noconfirm "$pkg" || { warn "could not install package: $pkg"; return 1; }
            ;;
        apt)
            DEBIAN_FRONTEND=noninteractive apt-get install -y "$pkg" || { warn "could not install package: $pkg"; return 1; }
            ;;
        dnf)
            dnf install -y "$pkg" || { warn "could not install package: $pkg"; return 1; }
            ;;
        zypper)
            zypper --non-interactive install --auto-agree-with-licenses --no-recommends "$pkg" || { warn "could not install package: $pkg"; return 1; }
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

normal_user() {
    local u="${SUDO_USER:-$USER}"
    if [[ "$u" == "root" || -z "$u" ]]; then
        die "run this script with sudo from a normal user, not as root directly"
    fi
    echo "$u"
}

inside_container() {
    [[ -f /.dockerenv ]] && return 0
    [[ -r /proc/1/cgroup ]] && grep -qaE '(docker|podman|containerd|kubepods|libpod)' /proc/1/cgroup && return 0
    if command -v systemd-detect-virt >/dev/null 2>&1; then
        systemd-detect-virt --container >/dev/null 2>&1 && return 0
    fi
    return 1
}

script_source_dir() {
    local src="${BASH_SOURCE[0]}"
    while [[ -L "$src" ]]; do
        local dir
        dir="$(cd -P "$(dirname "$src")" >/dev/null 2>&1 && pwd)"
        src="$(readlink "$src")"
        [[ "$src" != /* ]] && src="$dir/$src"
    done
    cd -P "$(dirname "$src")" >/dev/null 2>&1 && pwd
}


link_cmd() {
    local cmd="$1"
    local explicit_path="${2:-}"
    local target=""

    mkdir -p /usr/local/bin

    # Explicit path supplied by caller.
    if [[ -n "$explicit_path" && -x "$explicit_path" ]]; then
        target="$explicit_path"

    # Grace installed from source.
    elif [[ -x "/usr/local/grace/bin/$cmd" ]]; then
        target="/usr/local/grace/bin/$cmd"

    # Search common locations.
    else
        target="$({
            find \
                /usr/local/grace/bin \
                /usr/local/bin \
                /usr/bin \
                /usr/local \
                /usr \
                /opt/rakubrew \
                /opt \
                -type f -name "$cmd" -perm -111 \
                ! -path "/usr/local/bin/$cmd" \
                2>/dev/null || true
        } | head -n 1)"
    fi

    if [[ -n "$target" && -e "/usr/local/bin/$cmd" && "$target" -ef "/usr/local/bin/$cmd" ]]; then
        echo "✓ /usr/local/bin/$cmd already points to $target"
        return 0
    fi

    if [[ -n "$target" && -x "$target" ]]; then
        ln -sfn "$target" "/usr/local/bin/$cmd"
        chmod 755 "$target" 2>/dev/null || true
        echo "✓ /usr/local/bin/$cmd -> $target"
    elif command -v "$cmd" >/dev/null 2>&1; then
        echo "✓ $cmd already in PATH: $(command -v "$cmd")"
    else
        warn "$cmd not found; no link created"
    fi
}

link_grace_to_xmgrace() {
    local xg=""
    if [[ -x /usr/local/bin/xmgrace ]]; then
        xg="/usr/local/bin/xmgrace"
    elif command -v xmgrace >/dev/null 2>&1; then
        xg="$(command -v xmgrace)"
    else
        xg="$({ find /usr /usr/local -type f -name xmgrace -perm -111 2>/dev/null || true; } | head -n 1)"
    fi

    if [[ -n "$xg" && -x "$xg" ]]; then
#        ln -sf "$xg" /usr/local/bin/xmgrace
        ln -sf "$xg" /usr/local/bin/grace
        echo "✓ /usr/local/bin/grace -> $xg"
    else
        warn "xmgrace not found; cannot create grace -> xmgrace link"
    fi
}

setup_paths() {
    log "System PATH setup"

    mkdir -p /etc/profile.d /etc/sudoers.d

    cat >/etc/profile.d/ofe-path.sh <<'PROFILE_EOF'
# OneFit-Engine tools
for d in \
    /usr/local/bin \
    /usr/bin \
    /opt/rakubrew/bin \
    /opt/rakubrew/shims

do
    if [ -d "$d" ]; then
        case ":$PATH:" in
            *:"$d":*) ;;
            *) PATH="$d:$PATH" ;;
        esac
    fi
done

export RAKUBREW_HOME=${RAKUBREW_HOME:-/opt/rakubrew}
export PATH
PROFILE_EOF

    cat >/etc/sudoers.d/90-ofe-secure-path <<'SUDOERS_EOF'
Defaults secure_path=/usr/local/bin:/usr/bin:/opt/rakubrew/bin:/opt/rakubrew/shims:/usr/local/sbin:/usr/sbin:/sbin:/bin
Defaults env_keep += "RAKUBREW_HOME"
SUDOERS_EOF

    chmod 644 /etc/profile.d/ofe-path.sh
    chmod 440 /etc/sudoers.d/90-ofe-secure-path

    if command -v visudo >/dev/null 2>&1; then
        visudo -cf /etc/sudoers.d/90-ofe-secure-path || {
            warn "invalid sudoers PATH file; removing it"
            rm -f /etc/sudoers.d/90-ofe-secure-path
        }
    fi

    export RAKUBREW_HOME=/opt/rakubrew
    export PATH="/usr/local/bin:/usr/bin:/opt/rakubrew/bin:/opt/rakubrew/shims:$PATH"
}

install_repositories() {
    log "Repositories / package database"
    case "$OS_FAMILY" in
        arch)
            pacman -Sy --noconfirm || die "pacman package database refresh failed"
            ;;
        debian)
            apt-get update || die "apt-get update failed"
            install_pkg ca-certificates
            install_pkg apt-transport-https || true
            ;;
        fedora)
            install_pkg dnf-plugins-core || true
            dnf makecache || true
            ;;
        suse)
            zypper --non-interactive --gpg-auto-import-keys refresh || die "zypper refresh failed"
            ;;
    esac
}

install_packages_arch() {
    log "Arch packages"
    pacman -S --needed --noconfirm base-devel || warn "could not install base-devel group"
    local packages=(
        perl git openssh apache swig gcc-fortran make tar wget curl jq zip unzip
        sudo vim less man-db gnuplot openssl hdf5 poppler ghostscript imagemagick
        jre-openjdk-headless ffmpeg which
    )
    local pkg
    for pkg in "${packages[@]}"; do install_pkg "$pkg"; done
    install_first_available_pkg perl-cgi || true
    install_first_available_pkg xmgrace grace || true
    install_first_available_pkg pdftk pdftk-java || true
    install_first_available_pkg texlive-bin texlive-basic texlive || true
    install_first_available_pkg ufw firewalld || true
}

install_packages_debian() {
    log "Debian packages"
    local packages=(
        perl libperl-dev libcgi-pm-perl git openssh-server apache2 swig gcc gfortran
        make tar wget curl jq zip unzip sudo vim man-db gnuplot libssl-dev
        hdf5-tools libhdf5-dev poppler-utils texlive-font-utils ghostscript
        imagemagick default-jre-headless rakudo perl6-zef grace
    )
    local pkg
    for pkg in "${packages[@]}"; do install_pkg "$pkg"; done
    install_first_available_pkg pdftk-java pdftk || true
    install_first_available_pkg ffmpeg || true
    install_first_available_pkg ufw firewalld || true
}

enable_rpmfusion_free() {
    # Best-effort multimedia repository enablement for Fedora/RHEL-like systems.
    # Fedora uses /free/fedora/ and EL/CentOS/Rocky/Alma use /free/el/.
    if rpm -q rpmfusion-free-release >/dev/null 2>&1; then
        echo "✓ package installed: rpmfusion-free-release"
        return 0
    fi

    local fedora_version el_version url
    fedora_version="$(rpm -E %fedora 2>/dev/null || true)"

    if [[ -n "$fedora_version" && "$fedora_version" != "%fedora" ]]; then
        url="https://download1.rpmfusion.org/free/fedora/rpmfusion-free-release-${fedora_version}.noarch.rpm"
    else
        el_version="${OS_VERSION_ID%%.*}"
        [[ -z "$el_version" || "$el_version" == "unknown" ]] && el_version="10"
        url="https://download1.rpmfusion.org/free/el/rpmfusion-free-release-${el_version}.noarch.rpm"
    fi

    dnf install -y "$url" || warn "could not install RPM Fusion free release"
}

install_packages_fedora() {
    log "Fedora/RHEL packages"

    install_pkg dnf-plugins-core || true

    # CRB/PowerTools and EPEL are useful on RHEL-like systems. Harmless if absent.
    if [[ "$OS_ID" =~ ^(centos|centos-stream|rocky|almalinux|rhel|redhat|ol)$ ]]; then
        install_pkg epel-release || true
        dnf config-manager --set-enabled crb 2>/dev/null || true
        dnf config-manager --set-enabled powertools 2>/dev/null || true
    fi

    enable_rpmfusion_free || true

    local packages=(
        perl perl-devel perl-CGI git openssh-server httpd swig gcc gcc-gfortran
        make tar wget curl jq zip unzip sudo vim-enhanced man-db gnuplot openssl-devel
        hdf5 hdf5-devel poppler-utils ghostscript ImageMagick java-21-openjdk
        firewalld rakudo xmgrace diffutils
    )
    local pkg
    for pkg in "${packages[@]}"; do install_pkg "$pkg"; done
    install_first_available_pkg texlive-epstopdf texlive-epstopdf-bin || true
    install_first_available_pkg pdftk-java pdftk || true
    install_first_available_pkg ffmpeg ffmpeg-free || true
    install_first_available_pkg perl6-zef raku-zef zef || true
}

install_packages_suse() {
    log "openSUSE packages"
    local packages=(
        perl perl-CGI git openssh apache2 swig gcc gcc-fortran make tar
        wget curl jq zip unzip sudo vim man gnuplot openssl-devel hdf5 hdf5-devel
        poppler-tools ghostscript ImageMagick java-21-openjdk which gzip xz texlive-epstopdf-bin diffutils
    )
    local pkg
    for pkg in "${packages[@]}"; do install_pkg "$pkg"; done

    # Package names vary between Leap/Tumbleweed and enabled repositories.
    install_first_available_pkg xmgrace grace || true
    install_first_available_pkg pdftk pdftk-java || true
    install_first_available_pkg ffmpeg ffmpeg-8 ffmpeg-7 ffmpeg-6 || true
    install_first_available_pkg texlive-epstopdf-bin texlive-epstopdf texlive || true
    install_first_available_pkg firewalld SuSEfirewall2 || true
    install_first_available_pkg rakudo raku || true
    install_first_available_pkg zef perl6-zef raku-zef || true
}

install_packages() {
    case "$OS_FAMILY" in
        arch)   install_packages_arch ;;
        debian) install_packages_debian ;;
        fedora) install_packages_fedora ;;
        suse)   install_packages_suse ;;
    esac
}

setup_zef_with_rakubrew_only() {
    log "Install zef with Rakubrew helper"

    export RAKUBREW_HOME=/opt/rakubrew
    export PATH=/opt/rakubrew/bin:/opt/rakubrew/shims:/usr/local/bin:/usr/bin:$PATH

    if [[ ! -x /opt/rakubrew/bin/rakubrew ]]; then
        mkdir -p /opt
        curl -fsSL https://rakubrew.org/install-on-perl.sh | RAKUBREW_HOME=/opt/rakubrew sh
    fi

    link_cmd rakubrew /opt/rakubrew/bin/rakubrew
    rakubrew mode shim || true

    # Use system raku/rakudo if already installed
    link_cmd raku
    link_cmd rakudo

    # Build only zef, not another Rakudo
    rakubrew build-zef || {
 	   warn "rakubrew build-zef failed; falling back to full Rakubrew Rakudo + zef"
    	setup_raku_with_rakubrew
    	return
	}

    link_cmd zef /opt/rakubrew/shims/zef
}

setup_rakubrew_systemwide() {
    log "Install Rakubrew system-wide"
    setup_paths
    mkdir -p /opt

    if [[ ! -x /opt/rakubrew/bin/rakubrew ]]; then
        rm -rf /opt/rakubrew
        (set -o pipefail; curl -fsSL https://rakubrew.org/install-on-perl.sh | RAKUBREW_HOME=/opt/rakubrew sh) \
            || die "Rakubrew installation failed"
    else
        echo "✓ Rakubrew already installed: /opt/rakubrew/bin/rakubrew"
    fi

    export RAKUBREW_HOME=/opt/rakubrew
    export PATH=/opt/rakubrew/bin:/opt/rakubrew/shims:/usr/local/bin:/usr/bin:$PATH
    rakubrew mode shim || true
    link_cmd rakubrew /opt/rakubrew/bin/rakubrew
}

setup_raku_with_rakubrew() {
    log "Raku / zef via system-wide Rakubrew"
    setup_rakubrew_systemwide

    export RAKUBREW_HOME=/opt/rakubrew
    export PATH=/opt/rakubrew/bin:/opt/rakubrew/shims:/usr/local/bin:/usr/bin:$PATH

    if ! rakubrew versions 2>/dev/null | grep -q "moar-"; then
        rakubrew download || rakubrew build moar
    else
        echo "✓ Rakudo already installed under /opt/rakubrew"
    fi

    local version
    version="$(rakubrew versions 2>/dev/null | sed 's/^* *//' | awk '/moar-/ {print $1}' | sort -V | tail -n 1)"
    [[ -n "$version" ]] && rakubrew switch "$version"

    if ! command -v zef >/dev/null 2>&1 || ! zef --version >/dev/null 2>&1; then
        rakubrew build-zef || warn "rakubrew build-zef failed"
    fi

    chmod -R a+rX /opt/rakubrew 2>/dev/null || true
    link_cmd raku /opt/rakubrew/shims/raku
    link_cmd rakudo /opt/rakubrew/shims/rakudo
    link_cmd zef /opt/rakubrew/shims/zef
}

setup_raku_debian() {
    log "Debian Raku / zef"
    command -v raku >/dev/null 2>&1 || die "rakudo package did not provide raku"
    command -v zef  >/dev/null 2>&1 || die "perl6-zef package did not provide zef"
    link_cmd raku /usr/bin/raku
    link_cmd rakudo /usr/bin/rakudo
    link_cmd zef /usr/bin/zef
}

setup_raku_fedora_or_suse() {
    log "Fedora/openSUSE Raku / zef"

    if command -v raku >/dev/null 2>&1 && command -v zef >/dev/null 2>&1 && zef --version >/dev/null 2>&1; then
        echo "✓ raku found: $(command -v raku)"
        echo "✓ zef found:  $(command -v zef)"
        link_cmd raku
        link_cmd rakudo
        link_cmd zef
    else
        warn "system Raku/zef incomplete; using full Rakubrew fallback"
        setup_raku_with_rakubrew
    fi
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
    case "$OS_FAMILY" in
        arch)   setup_raku_with_rakubrew ;;
        debian) setup_raku_debian ;;
        fedora) setup_raku_fedora_or_suse ;;
        suse)   setup_raku_fedora_or_suse ;;
    esac
    install_raku_modules
}

setup_services() {
    log "Services"

    if inside_container; then
        echo "Container environment detected; skipping service enable/start."
        return 0
    fi

    if command -v systemctl >/dev/null 2>&1; then
        case "$OS_FAMILY" in
            debian)
                systemctl enable ssh apache2 >/dev/null 2>&1 || true
                systemctl start ssh apache2 >/dev/null 2>&1 || true
                ;;
            arch|fedora)
                systemctl enable sshd httpd >/dev/null 2>&1 || true
                systemctl start sshd httpd >/dev/null 2>&1 || true
                ;;
            suse)
                systemctl enable sshd apache2 >/dev/null 2>&1 || true
                systemctl start sshd apache2 >/dev/null 2>&1 || true
                ;;
        esac
    else
        warn "systemctl not found; skipping service enable/start"
    fi
}

install_grace_from_source() {
    log "Install Grace/xmgrace from source"

    if command -v xmgrace >/dev/null 2>&1; then
        echo "✓ xmgrace already found: $(command -v xmgrace)"
        link_cmd xmgrace
        link_cmd gracebat
        link_grace_to_xmgrace
        return 0
    fi

    # Dependencies for building Grace with the X/Motif interface.
    local build_packages=(
        base-devel openmotif texlive linuxdoc-tools git gcc make autoconf
        automake libtool pkgconf xorgproto libx11 libxext libxmu libxt
        libxpm freetype2 libpng libjpeg-turbo zlib
    )
    local pkg
    for pkg in "${build_packages[@]}"; do install_pkg "$pkg"; done

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

    cd "$tmpdir/xmgrace" || { rm -rf "$tmpdir"; die "could not enter Grace/xmgrace source directory"; }

    chmod +x ./configure 2>/dev/null || true

    set +e
    # GCC 14+ turns implicit-function-declaration and int-conversion into hard
    # errors by default, which breaks this old codebase's configure checks and
    # build unless explicitly downgraded back to warnings.
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

    link_cmd xmgrace 
    link_cmd gracebat
    link_grace_to_xmgrace

    echo "✓ Grace/xmgrace installed from source"
}

is_rhel_like_id() {
    [[ "$OS_ID" =~ ^(centos|centos-stream|rocky|almalinux|rhel|redhat|ol)$ ]]
}

setup_grace() {
    log "Grace / xmgrace"

    # Debian/Fedora/openSUSE usually install xmgrace/grace from packages.
    # If it is already present, just normalize the command links.
    if command -v xmgrace >/dev/null 2>&1; then
        echo "✓ xmgrace found: $(command -v xmgrace)"
        link_cmd xmgrace
        link_cmd gracebat
        link_grace_to_xmgrace
        return 0
    fi

    # Arch and RHEL-like systems often lack a usable xmgrace package.
    # Build from source only when the package path failed there.
    if [[ "$OS_FAMILY" == "arch" ]] || is_rhel_like_id; then
        warn "xmgrace package not available; building Grace from source"
        install_grace_from_source
        return 0
    fi

    warn "xmgrace not found; package installation may have failed"
    link_cmd xmgrace
    link_cmd gracebat
    link_grace_to_xmgrace
}

create_links() {
    log "Create /usr/local/bin links"
    local cmds=(raku rakudo zef prove6 xmgrace gracebat grace)
    [[ "$OS_FAMILY" == "arch" || -x /opt/rakubrew/bin/rakubrew ]] && cmds+=(rakubrew)

    local cmd
    for cmd in "${cmds[@]}"; do
        link_cmd "$cmd"
    done
    link_grace_to_xmgrace
}

open_ports() {
    log "Open port 8142 for OneFit-Engine"

    if command -v ufw >/dev/null 2>&1; then
        if ufw status 2>/dev/null | grep -q "Status: active"; then
            ufw allow 8142/tcp || warn "could not open port 8142 with ufw"
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
        else
            warn "firewalld is not active; skipping firewall configuration"
        fi
        return 0
    fi

    warn "no supported firewall tool found; skipping firewall configuration"
}

ofe_install() {
    log "Install OFE as normal user"
    local ofe_user
    ofe_user="$(normal_user)"

    local src_dir
    src_dir="$(script_source_dir)"

    # Development/source-tree mode: when this installer is inside an OFE tree,
    # install that exact local tree instead of cloning GitHub. This is important
    # for Docker/VM regression tests of uncommitted changes.
    if [[ -x "$src_dir/INSTALL" || -f "$src_dir/INSTALL" ]]; then
        log "Using local OFE source tree"
        echo "Source: $src_dir"
        chmod +x "$src_dir/INSTALL" 2>/dev/null || true
        sudo -u "$ofe_user" -H bash -lc "
            set -euo pipefail
            export RAKUBREW_HOME=/opt/rakubrew
            export PATH=/usr/local/bin:/usr/bin:/opt/rakubrew/bin:/opt/rakubrew/shims:\$PATH
            mkdir -p \"\$HOME/public_html\" \"\$HOME/.local\"
            cd '$src_dir'
            ./INSTALL -d
        "
        return 0
    fi

    # End-user mode: if the script is not being run from an OFE source tree,
    # clone/update the official repository under the normal user's home.
    sudo -u "$ofe_user" -H bash -lc '
        set -euo pipefail
        export RAKUBREW_HOME=/opt/rakubrew
        export PATH=/usr/local/bin:/usr/bin:/opt/rakubrew/bin:/opt/rakubrew/shims:$PATH

        mkdir -p "$HOME/public_html" "$HOME/.local"

        if [[ ! -d "$HOME/.local/OneFit-Engine/.git" ]]; then
            rm -rf "$HOME/.local/OneFit-Engine"
            git clone https://github.com/fitteia/OneFit-Engine.git "$HOME/.local/OneFit-Engine"
        else
            cd "$HOME/.local/OneFit-Engine"
            git pull --ff-only || true
        fi

        cd "$HOME/.local/OneFit-Engine"
        chmod +x ./INSTALL 2>/dev/null || true
        ./INSTALL -d
    '
}

run_ldconfig() {
    if command -v ldconfig >/dev/null 2>&1; then
        ldconfig "$@"
    elif [[ -x /sbin/ldconfig ]]; then
        /sbin/ldconfig "$@"
    elif [[ -x /usr/sbin/ldconfig ]]; then
        /usr/sbin/ldconfig "$@"
    else
        return 127
    fi
}

fix_libperl() {
    log "libperl dynamic-linker setup"

    local lib linkdir link
    lib="$(find /usr /opt -name libperl.so 2>/dev/null | head -n1)"

    [[ -z "$lib" ]] && {
        warn "libperl.so not found"
        return 1
    }

    if run_ldconfig -p 2>/dev/null | grep -q 'libperl\.so'; then
        echo "✓ libperl.so already visible to dynamic linker"
        return 0
    fi

    # Prefer registering the native Perl CORE directory. If ldconfig is absent,
    # fall back to a conventional lib64 symlink.
    local libdir
    libdir="$(dirname "$lib")"
    if [[ -d /etc/ld.so.conf.d ]] && run_ldconfig -p >/dev/null 2>&1; then
        echo "$libdir" >/etc/ld.so.conf.d/perl.conf
        run_ldconfig || true
        if run_ldconfig -p 2>/dev/null | grep -q 'libperl\.so'; then
            echo "✓ added $libdir to ldconfig path"
            return 0
        fi
    fi

    if [[ -d /usr/lib64 ]]; then
        linkdir=/usr/lib64
    else
        linkdir=/usr/lib
    fi
    link="$linkdir/libperl.so"

    if [[ ! -e "$link" ]]; then
        ln -s "$lib" "$link"
        echo "✓ $link -> $lib"
    else
        echo "✓ $link already exists"
    fi

    run_ldconfig >/dev/null 2>&1 || true
}

uninstall_ofe() {
    log "Uninstall OFE links and user tree"
    local ofe_user="${SUDO_USER:-$USER}"

    if [[ "$ofe_user" != "root" && -n "$ofe_user" ]]; then
        sudo -u "$ofe_user" -H bash -lc '
            rm -rf "$HOME/.local/OneFit-Engine"
            rm -f "$HOME/.local/bin/onefite" "$HOME/.local/bin/ofe" "$HOME/.local/bin/onefit" "$HOME/.local/bin/of"
        '
    fi

    rm -f /usr/local/bin/onefite /usr/local/bin/ofe /usr/local/bin/onefit /usr/local/bin/of
    rm -f /usr/local/bin/rakubrew /usr/local/bin/raku /usr/local/bin/rakudo /usr/local/bin/zef /usr/local/bin/prove6
    rm -f /usr/local/bin/xmgrace /usr/local/bin/gracebat /usr/local/bin/grace
    rm -f /etc/profile.d/ofe-path.sh /etc/sudoers.d/90-ofe-secure-path

    # arch always uses /opt/rakubrew; fedora/suse may have populated it via
    # the Rakubrew fallback path in setup_raku_fedora_or_suse.
    rm -rf /opt/rakubrew

    if command -v ufw >/dev/null 2>&1 && ufw status 2>/dev/null | grep -q "Status: active"; then
        ufw delete allow 8142/tcp >/dev/null 2>&1 || true
    fi
    if command -v firewall-cmd >/dev/null 2>&1; then
        firewall-cmd --permanent --remove-port=8142/tcp >/dev/null 2>&1 || true
        firewall-cmd --reload >/dev/null 2>&1 || true
    fi

    echo "OFE files and links removed. Packages installed by this script were not removed."
}

final_checks() {
    log "Final checks"
    local cmds=(
        perl git swig gcc gfortran make wget curl jq zip unzip
        sudo vim man gnuplot h5dump pdftk epstopdf ffmpeg
        raku rakudo zef prove6 xmgrace gracebat grace
    )
    [[ -x /opt/rakubrew/bin/rakubrew ]] && cmds+=(rakubrew)

    local service_cmds=(sshd "$WEB_SERVER_CMD")
    local cmd
    for cmd in "${cmds[@]}"; do
        [[ -z "$cmd" ]] && continue
        if command -v "$cmd" >/dev/null 2>&1; then
            printf "✓ %-10s %s\n" "$cmd" "$(command -v "$cmd")"
        else
            printf "✗ %-10s not found\n" "$cmd"
        fi
    done

    echo
    echo "Service commands:"
    for cmd in "${service_cmds[@]}"; do
        [[ -z "$cmd" ]] && continue
        if command -v "$cmd" >/dev/null 2>&1; then
            printf "✓ %-10s %s\n" "$cmd" "$(command -v "$cmd")"
        elif inside_container; then
            printf "- %-10s not found / not required in container\n" "$cmd"
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
			fix_libperl
            setup_raku_and_zef
            setup_services
            setup_grace
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
