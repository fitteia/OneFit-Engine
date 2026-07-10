#!/usr/bin/env bash

set -e

command -v onefite >/dev/null 2>&1 || exit 0

# Start the OFE service on non-macOS systems.
if [[ "$(uname -s)" != "Darwin" ]]; then
    echo "===> Starting OneFit-Engine service..."
    onefite service start
fi

echo
echo "===> Running OneFit-Engine self-test..."
onefite test

echo
echo "===> Post-install completed successfully."

