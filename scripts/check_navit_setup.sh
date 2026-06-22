#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PRODUCT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
NAVIT_CONFIG="${SERP_NAVIT_CONFIG:-$PRODUCT_ROOT/gen/integrations/navit/serp_navit.xml}"
NAVIT_MAP="${SERP_NAVIT_MAP:-$HOME/.local/share/serp/navit/maps/serp_demo.bin}"
NAVIT_TEXT_MAP="${SERP_NAVIT_TEXT_MAP:-$HOME/.local/share/serp/navit/maps/serp_demo.txt}"
if [[ "$(uname)" == "Darwin" ]]; then
  _default_plugin_root="$(brew --prefix navit 2>/dev/null)/lib/navit"
else
  _default_plugin_root="/usr/lib/navit"
fi
PLUGIN_ROOT="${SERP_NAVIT_PLUGIN_ROOT:-$_default_plugin_root}"

status=0

check_cmd() {
  local name="$1"
  if command -v "$name" >/dev/null 2>&1; then
    echo "ok: command '$name' -> $(command -v "$name")"
  else
    echo "missing: command '$name'"
    status=1
  fi
}

check_file() {
  local label="$1"
  local path="$2"
  if [[ -e "$path" ]]; then
    echo "ok: $label -> $path"
  else
    echo "missing: $label -> $path"
    status=1
  fi
}

check_cmd navit
check_cmd maptool
check_cmd xmllint
check_cmd curl
check_cmd gpsd

check_file "Navit gtk graphics plugin" "$PLUGIN_ROOT/graphics/libgraphics_gtk_drawing_area.so"
check_file "Navit internal GUI plugin" "$PLUGIN_ROOT/gui/libgui_internal.so"
check_file "Navit gtk GUI plugin" "$PLUGIN_ROOT/gui/libgui_gtk.so"
check_file "Navit binfile map plugin" "$PLUGIN_ROOT/map/libmap_binfile.so"

if [[ -e "$PLUGIN_ROOT/vehicle/libvehicle_gpsd.so" ]]; then
  echo "ok: Navit gpsd vehicle plugin -> $PLUGIN_ROOT/vehicle/libvehicle_gpsd.so"
elif [[ -e "$PLUGIN_ROOT/vehicle/libvehicle_pipe.so" || -e "$PLUGIN_ROOT/vehicle/libvehicle_socket.so" ]]; then
  echo "ok: Navit fallback vehicle plugin available under $PLUGIN_ROOT/vehicle"
else
  echo "missing: Navit gpsd/pipe/socket vehicle plugin under $PLUGIN_ROOT/vehicle"
  status=1
fi

check_file "SERP Navit config" "$NAVIT_CONFIG"
if [[ -f "$NAVIT_CONFIG" ]]; then
  if xmllint --noout "$NAVIT_CONFIG" >/dev/null 2>&1; then
    echo "ok: Navit config XML is valid"
  else
    echo "invalid: Navit config XML failed xmllint"
    status=1
  fi
  if grep -q '<plugins>' "$NAVIT_CONFIG"; then
    echo "ok: Navit config contains plugin loading block"
  else
    echo "missing: Navit config has no <plugins> block"
    status=1
  fi
fi

check_file "SERP Navit map" "$NAVIT_MAP"
if [[ -f "$NAVIT_MAP" ]]; then
  ls -lh "$NAVIT_MAP"
  if python3 - "$NAVIT_MAP" <<'PY'
import sys
import zipfile

try:
    with zipfile.ZipFile(sys.argv[1]) as archive:
        names = archive.namelist()
except zipfile.BadZipFile:
    raise SystemExit(1)

raise SystemExit(0 if any(name != "index" for name in names) else 2)
PY
  then
    echo "ok: Navit bin map contains tile data"
  else
    code=$?
    if [[ "$code" -eq 2 ]]; then
      echo "warning: Navit bin map contains only index data; textfile fallback is required"
    else
      echo "invalid: Navit bin map is not a readable bin/zip file"
      status=1
    fi
  fi
else
  echo "hint: create it with $SCRIPT_DIR/download_navit_map.sh"
fi

check_file "SERP Navit textfile fallback map" "$NAVIT_TEXT_MAP"
if [[ -f "$NAVIT_TEXT_MAP" ]]; then
  if [[ -s "$NAVIT_TEXT_MAP" ]]; then
    echo "ok: Navit textfile fallback map is non-empty"
  else
    echo "invalid: Navit textfile fallback map is empty"
    status=1
  fi
else
  echo "hint: create it with $SCRIPT_DIR/download_navit_map.sh"
fi

if [[ "$status" -ne 0 ]]; then
  cat <<'EOF'

Install/check hints:
  scripts/install_navit_ubuntu.sh
  scripts/download_navit_map.sh
EOF
  if [[ "$(uname)" == "Darwin" ]]; then
    echo "  export SERP_NAVIT_PLUGIN_ROOT=\"\$(brew --prefix navit)/lib/navit\""
  else
    echo "  export SERP_NAVIT_PLUGIN_ROOT=\"/usr/lib/navit\""
  fi
fi

exit "$status"
