#!/usr/bin/env bash
set -euo pipefail
SCRIPT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
source "$SCRIPT_DIR/lib/common.sh"

usage() {
  cat <<USAGE
Usage:
  $0 --deployment NAME
  $0 -d NAME

Required:
  -d, --deployment NAME    Available: monolith multiprocess_dbus_console multiprocess_dbus_qtquick multiprocess_grpc_qtquick
USAGE
}

deployment=""
while [[ $# -gt 0 ]]; do
  case "$1" in
    -d|--deployment) deployment="${2:-}"; shift 2 ;;
    -h|--help) usage; exit 0 ;;
    *) echo "Unknown option: $1" >&2; usage; exit 2 ;;
  esac
done

resolved=$(resolve_deployment "$deployment") || { echo "Invalid or unknown deployment: $deployment" >&2; usage; exit 2; }
build_dir=$(variant_dir "$resolved")
if [[ ! -d "$build_dir" ]]; then
  echo "Variant is not built: $build_dir" >&2
  exit 1
fi
runner="$DEPLOYMENTS_DIR/run_$(target_name "$resolved").sh"
if [[ ! -x "$runner" ]]; then
  echo "Generated runner is missing: $runner" >&2
  exit 1
fi
exec "$runner" "$build_dir"
