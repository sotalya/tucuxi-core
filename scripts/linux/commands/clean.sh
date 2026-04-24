#!/usr/bin/env bash
set -euo pipefail
source "$REPO_ROOT/scripts/linux/common/common.sh"

echo "==> Cleaning build directory"
rm -rf "$BUILD_ROOT"