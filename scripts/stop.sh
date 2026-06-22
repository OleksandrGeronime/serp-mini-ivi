#!/usr/bin/env bash
set -euo pipefail
SCRIPT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
source "$SCRIPT_DIR/lib/common.sh"

if pgrep -f "$BUILD_ROOT/variants" >/dev/null 2>&1; then
  pkill -TERM -f "$BUILD_ROOT/variants" || true
  sleep 1
  pkill -KILL -f "$BUILD_ROOT/variants" || true
else
  echo "No mini platform variant processes found."
fi
