#!/usr/bin/env bash
set -euo pipefail

source "$REPO_ROOT/scripts/linux/common/common.sh"
source "$REPO_ROOT/scripts/linux/common/python.sh"

cmd_coverage() {
  # Modules to build and test
  local -a MODULES=(core common query)

  local COVERAGE_ROOT="$REPO_ROOT/build/coverage"
  local BUILD_DIR="$COVERAGE_ROOT/gtest"
  local MERGED_INFO="$COVERAGE_ROOT/lcov/coverage.raw.info"
  local FILTERED_INFO="$COVERAGE_ROOT/lcov/coverage.info"
  local HTML_DIR="$COVERAGE_ROOT/html"

  # ---- options coverage (locales) ----
  local RUN_GTEST=1
  local RUN_FUZZ=0

  coverage_usage() {
    cat <<'EOF'
Usage:
  scripts/run coverage [options]

Default:
  Runs all test modules (tucucore, tucucommon, tucuquery) and merges coverage.

Options:
  --unittest           Run unittests (default ON unless --only is used)
  --no-unittest        Do not run unittests
  --fuzz               Add fuzz system tests

  -h, --help    Show this help

Modules built separately and merged:
  tucucore, tucucommon, tucuquery
EOF
  }

  # Parse args
  while [[ $# -gt 0 ]]; do
    case "$1" in
      -h|--help)
        echo "$BUILD_DIR"
        coverage_usage
        return 0
        ;;
      --unittest)
        RUN_GTEST=1
        shift
        ;;
      --no-unittest)
        RUN_GTEST=0
        shift
        ;;
      --fuzz)
        RUN_FUZZ=1
        shift
        ;;
      *) die "Unknown coverage option: $1 (try: ./scripts/linux/run coverage --help)" 2 ;;
    esac
  done

  if [[ $RUN_GTEST -eq 0 && $RUN_FUZZ -eq 0 ]]; then
    die "Nothing to run. Use --unittest and/or --fuzz/--imatinib/--cb-driver (or --all)." 2
  fi


  echo "==> Coverage: prerequisites"
  require_cmd cmake
  require_cmd lcov
  require_cmd genhtml

  lcov_version() {
    lcov --version | grep -Eo '([0-9]+\.[0-9]+(\.[0-9]+)?)' | head -n1
  }

  version_ge_115() {
    local v="$1"
    local major minor
    major="${v%%.*}"
    minor="${v#*.}"; minor="${minor%%.*}"
    [[ "$major" -gt 1 ]] || { [[ "$major" -eq 1 ]] && [[ "$minor" -ge 15 ]]; }
  }

  local LCOV_VER
  LCOV_VER="$(lcov_version)"

  local -a LCOV_IGNORE_ARGS=()
  if version_ge_115 "$LCOV_VER"; then
    echo "==> lcov version $LCOV_VER (>= 1.15): enabling --ignore-errors negative,mismatch,gcov"
    LCOV_IGNORE_ARGS=(--ignore-errors negative,mismatch,gcov)
  else
    echo "==> lcov version $LCOV_VER (< 1.15): running without --ignore-errors"
  fi

  echo "==> Coverage: clean coverage dirs"
  # rm -rf "$COVERAGE_ROOT"
  mkdir -p "$(dirname "$MERGED_INFO")" "$HTML_DIR"

  local -a ADD_TRACEFILES=()

  for MODULE in "${MODULES[@]}"; do
    local MOD_BUILD_DIR="$BUILD_DIR/tucu$MODULE"
    local MOD_SRC_DIR="$REPO_ROOT/test/tucu$MODULE"
    local MOD_INFO="$COVERAGE_ROOT/lcov/tucu$MODULE.info"
    local MOD_BIN="$MOD_BUILD_DIR/tucutest${MODULE}"

    if [[ $RUN_GTEST -eq 1 ]]; then
      echo "==> Coverage [$MODULE]: configure"
      cmake -S "$MOD_SRC_DIR" -B "$MOD_BUILD_DIR" \
        -Dconfig_coverage=ON \
        -DCMAKE_BUILD_TYPE=Debug

      echo "==> Coverage [$MODULE]: build"
      cmake --build "$MOD_BUILD_DIR" -j8

      echo "==> Coverage [$MODULE]: run tests"
      pushd "$MOD_BUILD_DIR"
      set +e
      "$MOD_BIN"
      local TEST_RC=$?
      set -e
      if [[ $TEST_RC -ne 0 ]]; then
        echo "==> WARN: $MODULE tests returned non-zero ($TEST_RC). Generating coverage anyway."
      fi
      popd
    else
      echo "==> Coverage: skipping unittests"
    fi

    echo "==> Coverage [$MODULE]: capture lcov"
    set +e
    lcov --capture \
      --directory "$MOD_BUILD_DIR" \
      --base-directory "$REPO_ROOT/src/" \
      "${LCOV_IGNORE_ARGS[@]}" \
      --no-external \
      --output-file "$MOD_INFO" >/dev/null
    local LCOV_RC=$?
    set -e
    if [[ $LCOV_RC -ne 0 ]]; then
      echo "==> WARN: lcov [$MODULE] returned non-zero ($LCOV_RC). Continuing."
    fi

    ADD_TRACEFILES+=(--add-tracefile "$MOD_INFO")
  done

  set +e
  if [[ $RUN_FUZZ -eq 1 ]]; then
    echo "==> Coverage: run fuzz system tests (python) using coverage build"
    "$REPO_ROOT/scripts/linux/run" fuzz --coverage
    FUZZ_RC=$?
  fi
  set -e

  if [[ $RUN_FUZZ -eq 1 && $FUZZ_RC -ne 0 ]]; then
    echo "==> WARN: fuzz returned non-zero ($FUZZ_RC)."
  fi

  echo "==> Coverage: merging reports"
  lcov "${ADD_TRACEFILES[@]}" --output-file "$MERGED_INFO" >/dev/null

  echo "==> Coverage: filter report (keep only project code)"
  lcov --extract "$MERGED_INFO" \
    "$REPO_ROOT/src/*" \
    --output-file "$FILTERED_INFO" >/dev/null

  echo "==> Coverage: generate HTML"
  rm -rf "$HTML_DIR"
  genhtml "$FILTERED_INFO" --output-directory "$HTML_DIR" >/dev/null

  echo "==> DONE: Coverage report at:"
  echo "   $HTML_DIR/index.html"
}