#!/usr/bin/env bash
set -euo pipefail
source "$REPO_ROOT/scripts/linux/common/common.sh"

echo "==> Running clang-format"
require_cmd clang-format

find "$REPO_ROOT/src" \
  \( -name '*.cpp' -o -name '*.hpp' -o -name '*.h' \) \
  -print0 | xargs -0 clang-format -i

echo "==> Format complete"