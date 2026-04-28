#!/usr/bin/env bash
set -euo pipefail

: "${REPO_ROOT:?REPO_ROOT must be set by entrypoint}"

: "${CONFIG:=debug}"
: "${COVERAGE_MODE:=0}"
: "${BUILD_ROOT:=$REPO_ROOT/build}"
: "${PYTHON_BIN:=python3}"

SCRIPT_DIR="$REPO_ROOT/scripts/linux/"

die() {
  local msg="$1"; local code="${2:-1}"
  echo "==> ERROR: $msg" >&2
  exit "$code"
}

require_cmd() {
  command -v "$1" >/dev/null 2>&1 || die "missing command '$1' (install it and retry)"
}

portable_nproc() {
  if command -v nproc >/dev/null 2>&1; then
    nproc
  elif sysctl -n hw.ncpu >/dev/null 2>&1; then
    sysctl -n hw.ncpu
  else
    echo 1
  fi
}

usage() {
  cat <<'EOF'
Usage:
  ./scripts/linux/run <command> [--debug|--release|--coverage]

By default, commands build in debug mode. Use --release for release mode, or --coverage to build with coverage flags (implies debug).

Commands:
  build           Configure + build main project (root CMake)
  build-unittest  Configure + build unittests (test/gtest CMake)
  run-unittest    Run unittests
  coverage        Run coverage (default: unittests only; see: ./scripts/linux/run coverage --help)
  format          Run clang-format on src
  tidy            Run clang-tidy (root compile_commands.json)
  clean           Remove build dir

  doc             Build documentation (html by default, or pdf)
                  Examples:
                    ./scripts/linux/run doc
                    ./scripts/linux/run doc html
                    ./scripts/linux/run doc pdf

  scan-build      Run clang static analyzer through scan-build
  codechecker     Run static analysis through CodeChecker

  py-env          Print activation command for .venv
  py-deps         Install python deps into .venv
  setup           Copy git hooks to .git/hooks
  fuzz            Run fuzzing
  imatinib        Run imatinib system tests
  cb-driver       Run covariate boundaries driver

  help
EOF
}

recompute_paths() {
  if [[ "${COVERAGE_MODE:-0}" == "1" ]]; then
    BUILD_ROOT="$REPO_ROOT/build/coverage"
    MAIN_BUILD_DIR="$BUILD_ROOT"
    GTEST_BUILD_DIR="$BUILD_ROOT/gtest"
  else
    BUILD_ROOT="$REPO_ROOT/build"
    MAIN_BUILD_DIR="$BUILD_ROOT/$CONFIG"
    GTEST_BUILD_DIR="$BUILD_ROOT/$CONFIG/gtest"
  fi

  DOC_BUILD_DIR="$BUILD_ROOT/docs"
  ANALYSIS_BUILD_DIR="$BUILD_ROOT/analysis"
  SCAN_BUILD_DIR="$ANALYSIS_BUILD_DIR/scan-build"
  CODECHECKER_DIR="$ANALYSIS_BUILD_DIR/codechecker"
  CODECHECKER_LOG="$CODECHECKER_DIR/compile_commands.json"
  CODECHECKER_REPORT_DIR="$CODECHECKER_DIR/reports"
  CODECHECKER_HTML_DIR="$CODECHECKER_DIR/html"
  ANALYSIS_SKIP_FILE="$ANALYSIS_BUILD_DIR/project.skip"

  TUCU_CLI="$MAIN_BUILD_DIR/tucucli/tucucli"
}

parse_global_opts() {
  while [[ $# -gt 0 ]]; do
    case "$1" in
      --debug)    CONFIG="debug"; shift ;;
      --release)  CONFIG="release"; shift ;;
      --coverage) COVERAGE_MODE=1; shift ;;
      *) break ;;
    esac
  done
  recompute_paths
}