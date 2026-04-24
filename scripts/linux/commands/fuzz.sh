#!/usr/bin/env bash
set -euo pipefail

source "$REPO_ROOT/scripts/linux/common/common.sh"
source "$REPO_ROOT/scripts/linux/common/cmake.sh"

cmd_fuzz() {
  echo "==> Running fuzzing ($CONFIG)"
  ensure_cli_built

  RUN_ID="$(date +%Y%m%d-%H%M%S)"
  if [[ "${COVERAGE_MODE:-0}" == "1" ]]; then
    OUT_DIR="$BUILD_ROOT/fuzzing/$RUN_ID"
  else
    OUT_DIR="$BUILD_ROOT/$CONFIG/fuzzing/$RUN_ID"
  fi

  mkdir -p "$OUT_DIR"

  pushd "$REPO_ROOT/test/system/fuzzing" >/dev/null

  "$PYTHON_BIN" fuzzing.py \
    --tucucli_path "$CDSS_CLI" \
    --drug_files_dir "$REPO_ROOT/test/system/drugfiles" \
    --original_input "$REPO_ROOT/test/system/fuzzing/imatinib.tqf" \
    --out_dir_path "$OUT_DIR/output" \
    --logfile_name "000_LOG.txt"

  popd >/dev/null

  echo "==> Output: $OUT_DIR/output"
}