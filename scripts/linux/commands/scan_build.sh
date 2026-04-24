#!/usr/bin/env bash
set -euo pipefail

source "$REPO_ROOT/scripts/linux/common/common.sh"
source "$REPO_ROOT/scripts/linux/common/cmake.sh"

cmd_scan_build() {
  require_cmd cmake
  require_cmd scan-build

  mkdir -p "$SCAN_BUILD_DIR"
  rm -rf "$MAIN_BUILD_DIR"
  mkdir -p "$MAIN_BUILD_DIR"

  echo "==> Configuring project with scan-build ($CONFIG)"
  scan-build \
    --status-bugs \
    --keep-going \
    -o "$SCAN_BUILD_DIR" \
    --exclude "$REPO_ROOT/libs" \
    --exclude "$REPO_ROOT/third_party" \
    cmake -S "$REPO_ROOT/make/cmake" -B "$MAIN_BUILD_DIR" \
      -DCMAKE_BUILD_TYPE="$(cmake_build_type)" \
      -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

  echo "==> Running scan-build"
  scan-build \
    --status-bugs \
    --keep-going \
    -o "$SCAN_BUILD_DIR" \
    --exclude "$REPO_ROOT/libs" \
    --exclude "$REPO_ROOT/third_party" \
    cmake --build "$MAIN_BUILD_DIR" --clean-first -j"$(portable_nproc)"
}