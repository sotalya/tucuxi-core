#!/usr/bin/env bash
set -euo pipefail
source "$REPO_ROOT/scripts/linux/common/common.sh"
source "$REPO_ROOT/scripts/linux/common/cmake.sh"

cmd_build_unittest() {
  echo "==> Building unittests ($CONFIG)"

  local -a extra=()
  local build_type
  build_type="$(cmake_build_type)"
  if [[ "${COVERAGE_MODE:-0}" == "1" ]]; then
    build_type="Debug"
    extra+=(-Dconfig_coverage=ON)
  fi

  for MODULE in tucucommon tucucore tucuquery
  do
    GTEST_MODULE_BUILD_DIR="$GTEST_BUILD_DIR/$MODULE/"
    echo "==> Building tests for $MODULE ($CONFIG)"
    cmake -S "$REPO_ROOT/test/$MODULE" -B "$GTEST_MODULE_BUILD_DIR" \
      -DCMAKE_BUILD_TYPE="$build_type" \
      "${extra[@]}"
    cmake --build "$GTEST_MODULE_BUILD_DIR" -j10
  done
}

cmd_run_unittest() {
  echo "==> Running unittestss ($CONFIG)"
  cmd_build_unittest

  for MODULE in common core query
  do
    GTEST_MODULE_BUILD_DIR="$GTEST_BUILD_DIR/tucu$MODULE/"
    echo "==> Running tests for $MODULE ($CONFIG)"
    "$GTEST_MODULE_BUILD_DIR/tucutest$MODULE" || true
  done
}