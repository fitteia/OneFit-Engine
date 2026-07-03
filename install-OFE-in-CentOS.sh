#!/usr/bin/env bash
#
# install-centos-ofe.sh
#
# CentOS Stream 10 installer helper for OneFit-Engine dependencies.
# It checks/install packages and creates links in /usr/local/bin for
# commands that may otherwise be outside normal users' PATH.

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

link_cmd() {
    local cmd="$1"
    local explicit_path="${2:-}"
    local target=""

    mkdir -p /usr/local/bin

    if [[ -n "$explicit_path" && -x "$explicit_path" ]]; then
        target="$explicit_path"
    else
        # Prefer locations outside /usr/local/bin to avoid linking to itself.
        target="$(
            find /opt /usr /usr/local \
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
# OneFit-Engine tools
for d in \
    /usr/local/bin \
    /opt/rakudo-pkg/bin \
    /opt/rakudo-pkg/share/perl6/site/bin \
    /opt/rakudo-pkg/share/perl6/vendor/bin \
    /opt/rakudo-pkg/share/perl6/core/bin \
    /usr/local/grace/bin \
    /usr/local/Grace/bin
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
Defaults secure_path=/usr/local/bin:/opt/rakudo-pkg/bin:/opt/rakudo-pkg/share/perl6/site/bin:/opt/rakudo-pkg/share/perl6/vendor/bin:/opt/rakudo-pkg/share/perl6/core/bin:/usr/local/grace/bin:/usr/local/Grace/bin:/usr/local/sbin:/usr/sbin:/usr/bin:/sbin:/bin
EOF
    chmod 440 /etc/sudoers.d/90-ofe-secure-path

    if command -v visudo >/dev/null 2>&1; then
        visudo -cf /etc/sudoers.d/90-ofe-secure-path || {
            warn "invalid sudoers PATH file; removing it"
            rm -f /etc/sudoers.d/90-ofe-secure-path
        }
    fi

    export PATH="/usr/local/bin:/opt/rakudo-pkg/bin:/opt/rakudo-pkg/share/perl6/site/bin:/opt/rakudo-pkg/share/perl6/vendor/bin:/opt/rakudo-pkg/share/perl6/core/bin:/usr/local/grace/bin:/usr/local/Grace/bin:$PATH"
}

install_repositories() {
    log "Repositories"

    install_pkg dnf-plugins-core
    install_pkg epel-release

    dnf config-manager --set-enabled crb 2>/dev/null || true
    dnf config-manager --set-enabled powertools 2>/dev/null || true

    if ! rpm -q rpmfusion-free-release >/dev/null 2>&1; then
        dnf install -y \
            https://download1.rpmfusion.org/free/el/rpmfusion-free-release-10.noarch.rpm \
            || warn "could not install RPM Fusion free release"
    else
        echo "✓ package installed: rpmfusion-free-release"
    fi
}

install_packages() {
    log "Packages"

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
        ImageMagick
        ghostscript
        zip
        unzip
        sudo
        curl
        jq
        vim-enhanced
        man-db
        gnuplot
        openssl-devel
        hdf5
        hdf5-devel
        poppler-utils
        pdftk-java
        texlive-epstopdf
        ffmpeg
        ffmpeg-devel
    )

    for pkg in "${packages[@]}"; do
        install_pkg "$pkg"
    done
}

install_rakudo_and_zef() {
    log "Rakudo / zef"

    setup_paths

    if ! command -v raku >/dev/null 2>&1; then
        curl -1sLf \
            'https://dl.cloudsmith.io/public/nxadm-pkgs/rakudo-pkg/setup.rpm.sh' \
            | bash
        install_pkg rakudo-pkg
    else
        echo "✓ raku found: $(command -v raku)"
    fi

    setup_paths
    link_cmd raku /opt/rakudo-pkg/bin/raku
    link_cmd rakudo /opt/rakudo-pkg/bin/rakudo

    # Install zef to the system site repository, not /root/.raku.
    if ! command -v zef >/dev/null 2>&1 || ! zef --version >/dev/null 2>&1; then
        echo "Installing zef system-wide"

        if [[ -x /opt/rakudo-pkg/bin/install-zef ]]; then
            # Force install-zef to use site instead of root's home when possible.
            cp /opt/rakudo-pkg/bin/install-zef /tmp/install-zef.ofe
            sed -i 's/--install-to=home/--install-to=site/g; s/install-to=home/install-to=site/g; s/"home"/"site"/g' /tmp/install-zef.ofe
            bash /tmp/install-zef.ofe || /opt/rakudo-pkg/bin/install-zef || true
        fi

        if ! command -v zef >/dev/null 2>&1 || ! zef --version >/dev/null 2>&1; then
            echo "Bootstrapping zef from source"
            rm -rf /tmp/zef-bootstrap
            git clone --depth 1 https://github.com/ugexe/zef.git /tmp/zef-bootstrap || true

            if [[ -f /tmp/zef-bootstrap/bin/zef ]]; then
                (
                    cd /tmp/zef-bootstrap || exit 1
                    /opt/rakudo-pkg/bin/raku -I. bin/zef install . --install-to=site --force-install --/test
                ) || true
            fi
        fi
    else
        echo "✓ zef found: $(command -v zef)"
    fi

    # Link the real zef if installed in a Raku bin directory.
    link_cmd zef
    chmod -R a+rX /opt/rakudo-pkg 2>/dev/null || true

    if command -v zef >/dev/null 2>&1; then
        zef update || true

        for mod in App::Prove6 Inline::Perl5 Readline; do
            echo "Installing Raku module: $mod"
            zef install --install-to=site --/test "$mod" || warn "could not install Raku module: $mod"
        done
    else
        warn "zef still not available; skipping Raku modules"
    fi

    link_cmd prove6
}

install_grace() {
    log "Grace / xmgrace"

    if command -v xmgrace >/dev/null 2>&1; then
        echo "✓ xmgrace found: $(command -v xmgrace)"
        link_cmd xmgrace
        link_cmd gracebat
        return 0
    fi

    cd /tmp || exit 1

    wget -nc https://plasma-gate.weizmann.ac.il/pub/grace/src/grace-latest.tar.gz || {
        warn "could not download Grace"
        return 1
    }

    local grace_dir
    grace_dir="$(tar tf grace-latest.tar.gz | head -n 1 | cut -d/ -f1)"

    rm -rf "$grace_dir"
    tar xf grace-latest.tar.gz

    cd "$grace_dir" || exit 1

    grep -q '#include <string.h>' src/rstdrv.c || \
        sed -i '/#include <stdlib.h>/a #include <string.h>' src/rstdrv.c

    make clean || true

    export CFLAGS="-std=gnu89 -Wno-implicit-int -Wno-implicit-function-declaration"

    ./configure --prefix=/usr/local
    make -j"$(nproc)"
    make install

    link_cmd grace
    link_cmd xmgrace
    link_cmd gracebat
}

create_links() {
    log "Create /usr/local/bin links"

    # command:optional explicit path
    local cmds=(
        raku:/opt/rakudo-pkg/bin/raku
        rakudo:/opt/rakudo-pkg/bin/rakudo
        zef
        grace:../grace/bin/grace
        gracebat:../grace/bin/gracebat
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

final_checks() {
    log "Final checks"

    local cmds=(
        perl git sshd httpd swig gcc gfortran make wget convert gs zip unzip
        sudo curl jq vim man gnuplot h5dump pdftk epstopdf ffmpeg
        raku zef prove6 xmgrace gracebat
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


open_ports() {
	echo "Open port 8142 for OneFit-Engine"

	firewall-cmd --permanent --add-port=8142/tcp
	firewall-cmd --reload
#	firewall-cmd --list-ports
}

ofe_install() {
	echo "=== Install OFE as normal user ==="

	OFE_USER="${SUDO_USER:-$USER}"

	if [[ "$OFE_USER" == "root" ]]; then
    	echo "ERROR: run this script with sudo from a normal user, not as root directly"
    	exit 1
	fi

	sudo -u "$OFE_USER" -H bash -lc '
    	set -e
    	./INSTALL -d --ip=0.0.0.0 -/w --/dpkg --/test
	'
}


setup_paths
install_repositories
install_packages
install_rakudo_and_zef
install_grace
create_links
final_checks
open_ports
ofe_install

echo
echo "Done."
