#!/usr/bin/env bash
set -euo pipefail
source "$REPO_ROOT/scripts/linux/common/common.sh"
source "$REPO_ROOT/scripts/linux/common/python.sh"

cmd_py_env() {
  ensure_venv
  echo "source \"$VENV_DIR/bin/activate\""
}

cmd_py_deps() {
  echo "==> Installing python deps into .venv"
  ensure_venv

  echo "==> Installing CodeChecker into .venv"
  "$VENV_PIP" install CodeChecker

  echo "==> Done."
  echo "==> To activate in your shell:"
  echo "   source \"$VENV_DIR/bin/activate\""
}