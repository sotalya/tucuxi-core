#!/usr/bin/env bash
set -euo pipefail

source "$REPO_ROOT/scripts/linux/common/common.sh"
source "$REPO_ROOT/scripts/linux/common/cmake.sh"

cmd_imatinib() {
  echo "==> imatinib system tests are not available in this project."
  return 0
}