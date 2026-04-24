#!/usr/bin/env bash
set -euo pipefail

source "$REPO_ROOT/scripts/linux/common/common.sh"
source "$REPO_ROOT/scripts/linux/common/cmake.sh"

cmd_cb_driver() {
  echo "==> cb-driver system tests are not available in this project."
  return 0
}