#!/usr/bin/env bash
set -euo pipefail
SCRIPT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
source "$SCRIPT_DIR/lib/common.sh"

usage() {
  cat <<USAGE
Usage:
  $0 --deployment NAME [options]
  $0 -d NAME [options]

Required:
  -d, --deployment NAME    Available: monolith multiprocess_dbus_console multiprocess_dbus_qtquick multiprocess_grpc_qtquick

Options:
  -jN                  Build with N jobs, for example -j4
  -jx                  Build with number of CPU cores
  -c, --clean          Remove selected build variant before configuring
  --runtime-mode MODE  Runtime debug backend: auto (default), grpc, dbus, none
  -h, --help           Show this help
USAGE
}

deployment=""
jobs="$(nproc 2>/dev/null || sysctl -n hw.logicalcpu 2>/dev/null || echo 2)"
clean=0
runtime_mode="auto"
while [[ $# -gt 0 ]]; do
  case "$1" in
    -d|--deployment) deployment="${2:-}"; shift 2 ;;
    -j[0-9]*) jobs="${1#-j}"; shift ;;
    -jx) jobs="$(nproc 2>/dev/null || sysctl -n hw.logicalcpu 2>/dev/null || echo 2)"; shift ;;
    -c|--clean) clean=1; shift ;;
    --runtime-mode) runtime_mode="${2:-auto}"; shift 2 ;;
    -h|--help) usage; exit 0 ;;
    *) echo "Unknown option: $1" >&2; usage; exit 2 ;;
  esac
done

resolved=$(resolve_deployment "$deployment") || { echo "Invalid or unknown deployment: $deployment" >&2; usage; exit 2; }
if needs_generation; then
  "$SCRIPT_DIR/generate.sh"
fi

build_dir=$(variant_dir "$resolved")
if [[ "$clean" == "1" ]]; then
  rm -rf "$build_dir"
fi
mkdir -p "$build_dir"

_PREFIX_PATH=$(cmake_prefix_path_arg)

cmake -U SERP_ROOT -S "$DEPLOYMENTS_DIR" -B "$build_dir" \
  ${_PREFIX_PATH:+-DCMAKE_PREFIX_PATH="$_PREFIX_PATH"} \
  -DSERP_PRODUCT_ROOT="$PRODUCT_ROOT" \
  -DSERP_DEPLOYMENT_NAME="$resolved" \
  -DSERP_TARGETS_FILES="$(target_files_arg)" \
  -DSERP_INSTANTIATE_DEPLOYMENT_COMPONENTS=ON \
  -DSERP_ENABLE_STANDALONE_FALLBACK_TARGETS=OFF \
  -DSERP_RUNTIME_MODE="$runtime_mode"

cmake --build "$build_dir" -j"$jobs"
echo "Built deployment: $resolved"
echo "Build dir: $build_dir"
