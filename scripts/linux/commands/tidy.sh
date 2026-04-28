#!/usr/bin/env bash
set -euo pipefail
source "$REPO_ROOT/scripts/linux/common/common.sh"
source "$REPO_ROOT/scripts/linux/common/cmake.sh"

echo "==> Running clang-tidy"
require_cmd clang-tidy
require_cmd run-clang-tidy

echo "==> Building project ensuring compile_commands.json exists"
source "$REPO_ROOT/scripts/linux/commands/build.sh"

for MODULE in tucucommon tucucore tucuquery tucucli tucudrugfilechecker
do
  REPORT_FILE="$BUILD_ROOT/$MODULE-tidy-report.txt"

  echo "==> Running analysis for $MODULE (output -> $REPORT_FILE)"
  run-clang-tidy -quiet -p "$MAIN_BUILD_DIR/$MODULE" \
    "$REPO_ROOT/src/$MODULE" \
    2>&1 | sed '/^clang-tidy/d; /warnings generated\.$/d' | sed $'s/\x1B\[[0-9;]*[A-Za-z]//g' > "$REPORT_FILE"
done

echo "==> Tidy complete"