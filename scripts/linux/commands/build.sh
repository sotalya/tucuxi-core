#!/usr/bin/env bash
set -euo pipefail
source "$REPO_ROOT/scripts/linux/common/common.sh"
source "$REPO_ROOT/scripts/linux/common/cmake.sh"

if [[ -v MODULES ]]; then
  echo "==> Building specified modules: ${MODULES[*]}"
else
  local -a MODULES=(tucucli tucudrugfilechecker)
fi

for MODULE in "${MODULES[@]}"
do
  MODULE_BUILD_DIR="$BUILD_ROOT/$CONFIG/$MODULE/"
  echo "==> Building $MODULE ($CONFIG)"
  cmake -S "$REPO_ROOT/src/$MODULE" -B "$MODULE_BUILD_DIR" \
    -DCMAKE_BUILD_TYPE="$(cmake_build_type)" \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
  cmake --build "$MODULE_BUILD_DIR" -j"$(portable_nproc)"
done
