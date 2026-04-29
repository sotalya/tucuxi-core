#!/usr/bin/env bash
set -euo pipefail
source "$REPO_ROOT/scripts/linux/common/common.sh"
source "$REPO_ROOT/scripts/linux/common/cmake.sh"

local -a MODULES=(tucucommon tucucore tucuquery)
local -a TEST_BINARIES=(tucutestcommon tucutestcore tucutestquery)

cmd_build_unittest() {
  echo "==> Building unittests ($CONFIG)"

  local -a extra=()
  local build_type
  build_type="$(cmake_build_type)"
  if [[ "${COVERAGE_MODE:-0}" == "1" ]]; then
    build_type="Debug"
    extra+=(-Dconfig_coverage=ON)
  fi

  for MODULE in "${MODULES[@]}"
  do
    local GTEST_MODULE_BUILD_DIR="$GTEST_BUILD_DIR/$MODULE/"
    echo "==> Building tests for $MODULE ($CONFIG)"
    cmake -S "$REPO_ROOT/test/$MODULE" -B "$GTEST_MODULE_BUILD_DIR" \
      -DCMAKE_BUILD_TYPE="$build_type" \
      "${extra[@]}"
    cmake --build "$GTEST_MODULE_BUILD_DIR" -j"$(portable_nproc)"
  done
}

cmd_run_unittest() {
  echo "==> Running unittestss ($CONFIG)"
  cmd_build_unittest

  local i=0
  for MODULE in "${MODULES[@]}"
  do
    local GTEST_MODULE_BUILD_DIR="$GTEST_BUILD_DIR/$MODULE/"
    echo "==> Running tests for $MODULE ($CONFIG)"

    pushd "$GTEST_MODULE_BUILD_DIR" >/dev/null
    "$GTEST_MODULE_BUILD_DIR/${TEST_BINARIES[$i]}" || true
    popd >/dev/null
    i=$((i + 1))
  done
}