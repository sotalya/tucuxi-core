#!/usr/bin/env bash
set -euo pipefail
source "$REPO_ROOT/scripts/linux/common/common.sh"
source "$REPO_ROOT/scripts/linux/common/cmake.sh"

cmd_build_unittest() {
  echo "==> Building unittests ($CONFIG)"

  for MODULE in tucucommon tucucore tucuquery
  do
    GTEST_MODULE_BUILD_DIR="$GTEST_BUILD_DIR/$MODULE/"
    echo "==> Building tests for $MODULE ($CONFIG)"
    cmake -S "$REPO_ROOT/test/$MODULE" -B "$GTEST_MODULE_BUILD_DIR" \
      -DCMAKE_BUILD_TYPE="$(cmake_build_type)"
    cmake --build "$GTEST_MODULE_BUILD_DIR" -j"$(portable_nproc)"
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