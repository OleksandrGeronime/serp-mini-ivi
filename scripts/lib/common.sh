#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
PRODUCT_ROOT=$(cd "$SCRIPT_DIR/.." && pwd)

GEN_DIR="$PRODUCT_ROOT/build/gen"
DEPLOYMENTS_DIR="$GEN_DIR/deployments"
BUILD_ROOT="$PRODUCT_ROOT/build"
MANIFEST="$BUILD_ROOT/.serp_manifest.json"
GEN_MANIFEST="$BUILD_ROOT/.serp_generation.json"
AVAILABLE_DEPLOYMENTS="monolith multiprocess_dbus_console multiprocess_dbus_qtquick multiprocess_grpc_qtquick"
TARGET_FILES=("$PRODUCT_ROOT/build/gen/services/SerpServicesTargets.cmake")

cmake_prefix_path_arg() {
  local parts=()
  [[ -n "${CMAKE_PREFIX_PATH:-}" ]] && parts+=("${CMAKE_PREFIX_PATH}")
  if [[ "$(uname)" == "Darwin" ]]; then
    local brew_qt=""
    brew_qt=$(brew --prefix qt 2>/dev/null || true)
    [[ -n "$brew_qt" && -d "$brew_qt" ]] && parts+=("$brew_qt")
  fi
  local IFS=';'
  echo "${parts[*]:-}"
}

resolve_deployment() {
  case "$1" in
    monolith) echo "monolith" ;;
    multiprocess_dbus_console) echo "multiprocess_dbus_console" ;;
    multiprocess_dbus_qtquick) echo "multiprocess_dbus_qtquick" ;;
    multiprocess_grpc_qtquick) echo "multiprocess_grpc_qtquick" ;;
    *) return 1 ;;
  esac
}

target_name() {
  echo "$1" | tr '[:upper:]' '[:lower:]' | sed -E 's/[^a-z0-9]+/_/g; s/^_+//; s/_+$//'
}

variant_dir() {
  local deployment="$1"
  echo "$BUILD_ROOT/variants/$(target_name "$deployment")"
}

target_files_arg() {
  local existing=()
  local item
  for item in "${TARGET_FILES[@]}"; do
    if [[ -f "$item" ]]; then
      existing+=("$item")
    fi
  done
  local IFS=';'
  echo "${existing[*]}"
}

needs_generation() {
  # If generated output doesn't exist, always regenerate
  if [[ ! -d "$GEN_DIR" ]]; then
    return 0
  fi

  # If generation manifest doesn't exist, always regenerate
  if [[ ! -f "$GEN_MANIFEST" ]]; then
    return 0
  fi

  # Compare spec file mtimes from manifest vs current
  # Extract spec paths and mtimes from JSON and check for changes
  python3 << 'PYTHON_EOF'
import json
from pathlib import Path

manifest_path = "$GEN_MANIFEST"
product_root = "$PRODUCT_ROOT"

try:
  with open(manifest_path) as f:
    manifest = json.load(f)

  old_specs = manifest.get('specs', {})

  # Check each spec file's mtime
  for spec_path, old_mtime in old_specs.items():
    full_path = Path(product_root) / spec_path
    if full_path.exists():
      current_mtime = full_path.stat().st_mtime
      if current_mtime != old_mtime:
        exit(0)  # Spec changed, need regeneration
    else:
      exit(0)  # Spec file deleted, need regeneration

  # Check if new spec files appeared
  import os
  for root, dirs, files in os.walk(Path(product_root) / 'specs'):
    for f in files:
      spec_path = str(Path(root) / f)
      rel_path = str(Path(spec_path).relative_to(product_root))
      if rel_path not in old_specs:
        exit(0)  # New spec file, need regeneration

  exit(1)  # No changes detected
except:
  exit(0)  # On error, regenerate to be safe
PYTHON_EOF
}
