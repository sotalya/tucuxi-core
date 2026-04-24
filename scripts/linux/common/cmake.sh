#!/usr/bin/env bash
set -euo pipefail

# shellcheck source=/dev/null
source "$REPO_ROOT/scripts/linux/common/common.sh"

cmake_build_type() {
  case "$CONFIG" in
    debug) echo "Debug" ;;
    release) echo "Release" ;;
    *) echo "Debug" ;;
  esac
}

ensure_cli_built() {
  if [[ ! -x "$CDSS_CLI" ]]; then
    echo "==> CDSS CLI not found, building first..."

    local -a extra=()
    local build_type
    build_type="$(cmake_build_type)"

    if [[ "${COVERAGE_MODE:-0}" == "1" ]]; then
      build_type="Debug"
      extra+=(-Dconfig_coverage=ON)
    fi

    cmake -S "$REPO_ROOT/make/cmake" -B "$MAIN_BUILD_DIR" \
      -DCMAKE_BUILD_TYPE="$build_type" \
      -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
      "${extra[@]}"

    cmake --build "$MAIN_BUILD_DIR" -j"$(portable_nproc)"
  fi
}

# ensure_cli_built() {
#   if [[ ! -x "$CDSS_CLI" ]]; then
#     echo "==> CDSS CLI not found, building first..."
#     cmake -S "$REPO_ROOT" -B "$MAIN_BUILD_DIR" \
#       -DCMAKE_BUILD_TYPE="$(cmake_build_type)" \
#       -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
#     cmake --build "$MAIN_BUILD_DIR" -j"$(nproc)"
#   fi
# }