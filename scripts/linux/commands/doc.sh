#!/usr/bin/env bash
set -euo pipefail

source "$REPO_ROOT/scripts/linux/common/common.sh"
source "$REPO_ROOT/scripts/linux/common/python.sh"

cmd_doc() {
  local format="${1:-html}"

  require_cmd doxygen

  mkdir -p "$DOC_BUILD_DIR"

  case "$format" in
    html)
      echo "==> Building documentation with Doxygen"
      (cd "$REPO_ROOT/src" && TUCUXI_ROOT="$REPO_ROOT" DOC_OUTPUT_DIR="$DOC_BUILD_DIR" doxygen doxyfile)
      echo "==> Docs generated: $DOC_BUILD_DIR/html/index.html"
      ;;
    -h|--help|help)
      cat <<'EOF'
Usage:
  ./scripts/linux/run doc

Runs Doxygen using src/doxyfile.
EOF
      ;;
    *)
      die "Unknown doc format: $format"
      ;;
  esac
}