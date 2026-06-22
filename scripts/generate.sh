#!/usr/bin/env bash
set -euo pipefail
SCRIPT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
PRODUCT_ROOT=$(cd "$SCRIPT_DIR/.." && pwd)

# Locate serpgen: PATH first, then SERP_ROOT dist, then SERP_ROOT Python launcher.
if command -v serpgen &>/dev/null; then
  SERP_GEN="serpgen"
elif [[ -n "${SERP_ROOT:-}" && -x "${SERP_ROOT}/dist/serpgen/core/serpgen" ]]; then
  SERP_GEN="${SERP_ROOT}/dist/serpgen/core/serpgen"
elif [[ -n "${SERP_ROOT:-}" && -f "${SERP_ROOT}/serpgen/serp_app_generator.py" ]]; then
  SERP_GEN="python3 ${SERP_ROOT}/serpgen/serp_app_generator.py"
else
  echo "Error: serpgen not found." >&2
  echo "  Add it to PATH or set SERP_ROOT to your SERP repository clone." >&2
  exit 1
fi

$SERP_GEN generate-workspace \
  -w "$PRODUCT_ROOT/specs/serp.sidl"
