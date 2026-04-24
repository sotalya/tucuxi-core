#!/usr/bin/env bash
set -euo pipefail
source "$REPO_ROOT/scripts/linux/common/common.sh"
source "$REPO_ROOT/scripts/linux/common/cmake.sh"

echo "==> Running clang-tidy"
require_cmd clang-tidy
require_cmd run-clang-tidy

REPORT_FILE="$BUILD_ROOT/tidy-report.txt"

echo "==> Ensuring compile_commands.json exists"
cmake -S "$REPO_ROOT/make/cmake" -B "$MAIN_BUILD_DIR" \
  -DCMAKE_BUILD_TYPE="$(cmake_build_type)" \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

echo "==> Running analysis (output -> $REPORT_FILE)"
run-clang-tidy -quiet -p "$MAIN_BUILD_DIR" \
  "$REPO_ROOT/src/tucucore" \
  "$REPO_ROOT/src/tucucommon" \
  "$REPO_ROOT/src/tucuquery" \
  2>&1 | sed '/^clang-tidy/d; /warnings generated\.$/d' | sed $'s/\x1B\[[0-9;]*[A-Za-z]//g' > "$REPORT_FILE"

echo "==> Tidy complete"