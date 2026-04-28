#!/usr/bin/env bash
set -euo pipefail

source "$REPO_ROOT/scripts/linux/common/common.sh"
source "$REPO_ROOT/scripts/linux/common/cmake.sh"

write_codechecker_skipfile() {
  mkdir -p "$ANALYSIS_BUILD_DIR"

  cat > "$ANALYSIS_SKIP_FILE" <<EOF
+$REPO_ROOT/src/*
+$REPO_ROOT/test/*
-$REPO_ROOT/libs/*
-$REPO_ROOT/third_party/*
-*
EOF

  echo "==> Skip file written to: $ANALYSIS_SKIP_FILE"
}

cmd_codechecker() {
  require_cmd cmake
  source "$REPO_ROOT/scripts/linux/common/python.sh"
  ensure_venv

  CODECHECKER_BIN="$VENV_DIR/bin/CodeChecker"
  [[ -x "$CODECHECKER_BIN" ]] || die "CodeChecker is not installed in .venv. Run: ./scripts/linux/run py-deps"

  mkdir -p "$CODECHECKER_DIR"
  mkdir -p "$CODECHECKER_REPORT_DIR"
  mkdir -p "$CODECHECKER_HTML_DIR"
  mkdir -p "$MAIN_BUILD_DIR"

  write_codechecker_skipfile

  for MODULE in tucucommon tucucore tucuquery tucucli tucudrugfilechecker
  do
    mkdir -p "$MAIN_BUILD_DIR/$MODULE"
    mkdir -p "$CODECHECKER_REPORT_DIR/$MODULE"
    mkdir -p "$CODECHECKER_HTML_DIR/$MODULE"

    echo "==> Configuring $MODULE project for CodeChecker ($CONFIG)"
    cmake -S "$REPO_ROOT/src/$MODULE" -B "$MAIN_BUILD_DIR/$MODULE" \
      -DCMAKE_BUILD_TYPE="$(cmake_build_type)" \
      -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

    rm -f "$CODECHECKER_LOG"
    rm -rf "$CODECHECKER_REPORT_DIR/$MODULE" "$CODECHECKER_HTML_DIR/$MODULE"
    mkdir -p "$CODECHECKER_REPORT_DIR/$MODULE" "$CODECHECKER_HTML_DIR/$MODULE"

    echo "==> Recording build commands"
    "$CODECHECKER_BIN" log \
      -o "$CODECHECKER_LOG" \
      -b "cmake --build \"$MAIN_BUILD_DIR/$MODULE\" --clean-first -j$(portable_nproc)"

    echo "==> Running analysis"
    "$CODECHECKER_BIN" analyze \
      "$CODECHECKER_LOG" \
      --analyzers clangsa \
      --skip "$ANALYSIS_SKIP_FILE" \
      -o "$CODECHECKER_REPORT_DIR/$MODULE"

    echo "==> Generating HTML report"
    "$CODECHECKER_BIN" parse \
      -e html \
      "$CODECHECKER_REPORT_DIR/$MODULE" \
      -o "$CODECHECKER_HTML_DIR/$MODULE"

    if [[ -f "$CODECHECKER_HTML_DIR/$MODULE/index.html" ]]; then
      echo "==> CodeChecker report: $CODECHECKER_HTML_DIR/$MODULE/index.html"
    else
      echo "==> CodeChecker finished. Check $CODECHECKER_HTML_DIR/$MODULE"
    fi
  done
}