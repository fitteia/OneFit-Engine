#!/usr/bin/env bash
#
# pre-install-OFE-in-MacOS.sh
#
# Minimal macOS pre-installer for OneFit-Engine.
# It prepares the basic toolchain and Raku/zef environment.
# The main OFE installation remains the responsibility of ./INSTALL.
#
# Run:
#   ./pre-install-OFE-in-MacOS.sh
#   ./INSTALL -d
#

set -euo pipefail

log()  { echo; echo "=== $* ==="; }
warn() { echo "WARNING: $*" >&2; }
die()  { echo "ERROR: $*" >&2; exit 1; }

[[ "$(uname -s)" == "Darwin" ]] || die "This pre-installer is for macOS only."

if ! command -v brew >/dev/null 2>&1; then
    die "Homebrew is not installed. Install it first from https://brew.sh"
fi

BREW_PREFIX="$(brew --prefix)"

install_brew_pkg() {
    local pkg="$1"
    if brew list --formula "$pkg" >/dev/null 2>&1; then
        echo "✓ $pkg already installed"
    else
        echo "Installing $pkg"
        brew install "$pkg"
    fi
}

link_cmd() {
    local cmd="$1"
    local target="${2:-}"

    if command -v "$cmd" >/dev/null 2>&1; then
        echo "✓ $cmd found: $(command -v "$cmd")"
        return 0
    fi

    if [[ -n "$target" && -x "$target" ]]; then
        sudo mkdir -p /usr/local/bin
        sudo ln -sfn "$target" "/usr/local/bin/$cmd"
        echo "✓ /usr/local/bin/$cmd -> $target"
        return 0
    fi

    warn "$cmd not found"
    return 1
}

log "Homebrew packages"

# Keep this intentionally small. The main ./INSTALL script handles the rest.
install_brew_pkg rakudo
install_brew_pkg gcc
install_brew_pkg git

log "Raku / zef"

link_cmd raku || die "raku not found after installing rakudo"

if command -v zef >/dev/null 2>&1 && zef --version >/dev/null 2>&1; then
    echo "✓ zef found: $(command -v zef)"
else
    echo "Installing zef from source"
    mkdir -p "$HOME/.raku"

    if [[ ! -d "$HOME/.raku/zef/.git" ]]; then
        rm -rf "$HOME/.raku/zef"
        git clone https://github.com/ugexe/zef.git "$HOME/.raku/zef"
    else
        git -C "$HOME/.raku/zef" pull --ff-only || true
    fi

    (
        cd "$HOME/.raku/zef"
        raku -I. bin/zef install .
    )
fi

# Homebrew/Raku has used different site-bin paths over time. Try the common ones.
ZEF_TARGET=""
for candidate in \
    "$BREW_PREFIX/share/perl6/site/bin/zef" \
    "$BREW_PREFIX/share/raku/site/bin/zef" \
    "$HOME/.raku/bin/zef" \
    "$HOME/.raku/share/perl6/site/bin/zef" \
    "$HOME/.raku/share/raku/site/bin/zef"
do
    if [[ -x "$candidate" ]]; then
        ZEF_TARGET="$candidate"
        break
    fi
done

if ! command -v zef >/dev/null 2>&1; then
    if [[ -n "$ZEF_TARGET" ]]; then
        link_cmd zef "$ZEF_TARGET"
    else
        warn "zef was installed but no executable was found in the expected locations"
    fi
fi

log "Final checks"
for cmd in brew git gcc gfortran raku zef; do
    if command -v "$cmd" >/dev/null 2>&1; then
        printf "✓ %-8s %s\n" "$cmd" "$(command -v "$cmd")"
    else
        printf "✗ %-8s not found\n" "$cmd"
    fi
done

echo
echo "macOS pre-installation completed."
echo "Now run:"
echo "  ./INSTALL -d"
