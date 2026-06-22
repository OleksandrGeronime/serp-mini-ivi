#!/usr/bin/env bash
set -euo pipefail

AREA="${1:-tiny}"

MAP_ROOT="${SERP_NAVIT_MAP_ROOT:-$HOME/.local/share/serp/navit/maps}"
CACHE_DIR="$MAP_ROOT/cache"
OSM_FILE="$CACHE_DIR/${AREA}.osm"
NORMALIZED_OSM="$CACHE_DIR/${AREA}.normalized.osm"
BIN_FILE="$MAP_ROOT/serp_demo.bin"
TEXT_FILE="$MAP_ROOT/serp_demo.txt"
MAPTOOL_LOG="$CACHE_DIR/${AREA}.maptool.log"

mkdir -p "$CACHE_DIR" "$MAP_ROOT"

need_cmd() {
  command -v "$1" >/dev/null 2>&1 || {
    echo "ERROR: missing command: $1" >&2
    echo "Install:" >&2
    if [[ "$(uname)" == "Darwin" ]]; then
      echo "  brew install navit curl libxml2" >&2
    else
      echo "  sudo apt-get update && sudo apt-get install -y navit curl libxml2-utils" >&2
    fi
    exit 1
  }
}

if [[ "$AREA" == "clean" ]]; then
  rm -f "$CACHE_DIR"/*.osm "$CACHE_DIR"/*.osm.pbf "$CACHE_DIR"/*.normalized.osm "$MAP_ROOT"/*.bin "$MAP_ROOT"/*.txt
  echo "Cleaned Navit demo maps in $MAP_ROOT"
  exit 0
fi

need_cmd curl
need_cmd maptool
need_cmd xmllint
need_cmd python3

case "$AREA" in
  tiny)
    BBOX="42.500,-83.565,42.585,-83.430"
    ;;
  wixom|demo|default)
    BBOX="42.420,-83.575,42.615,-83.300"
    ;;
  novi)
    BBOX="42.420,-83.565,42.525,-83.330"
    ;;
  farmington)
    BBOX="42.430,-83.450,42.560,-83.300"
    ;;
  *)
    echo "ERROR: unknown area: $AREA" >&2
    echo "Supported: tiny, wixom, novi, farmington, clean" >&2
    exit 1
    ;;
esac

OVERPASS_URL="${SERP_OVERPASS_URL:-https://overpass.kumi.systems/api/interpreter}"

QUERY_FILE="$(mktemp)"
trap 'rm -f "$QUERY_FILE"' EXIT

cat > "$QUERY_FILE" <<EOF
[out:xml][timeout:180];
(
  way["highway"](${BBOX});
  node["place"](${BBOX});
  node["amenity"~"fuel|parking|restaurant|cafe|fast_food|hospital|pharmacy"](${BBOX});
);
(._;>;);
out body;
EOF

navit_bin_has_tiles() {
  local map_file="$1"
  python3 - "$map_file" <<'PY'
import sys
import zipfile

try:
    with zipfile.ZipFile(sys.argv[1]) as archive:
        names = archive.namelist()
except zipfile.BadZipFile:
    raise SystemExit(1)

raise SystemExit(0 if any(name != "index" for name in names) else 1)
PY
}

write_textfile_map() {
  local source_osm="$1"
  local output_txt="$2"
  python3 - "$source_osm" "$output_txt" <<'PY'
from __future__ import annotations

import sys
import xml.etree.ElementTree as ET
from pathlib import Path

source = Path(sys.argv[1])
target = Path(sys.argv[2])

nodes: dict[str, tuple[str, str, dict[str, str]]] = {}
ways: list[tuple[dict[str, str], list[str]]] = []

for _event, elem in ET.iterparse(source, events=("end",)):
    if elem.tag == "node":
        tags = {tag.get("k", ""): tag.get("v", "") for tag in elem.findall("tag")}
        node_id = elem.get("id")
        lat = elem.get("lat")
        lon = elem.get("lon")
        if node_id and lat and lon:
            nodes[node_id] = (lat, lon, tags)
        elem.clear()
    elif elem.tag == "way":
        tags = {tag.get("k", ""): tag.get("v", "") for tag in elem.findall("tag")}
        refs = [nd.get("ref", "") for nd in elem.findall("nd")]
        if tags.get("highway") and len(refs) >= 2:
            ways.append((tags, refs))
        elem.clear()

street_type_by_highway = {
    "motorway": "street_4_land",
    "trunk": "street_4_land",
    "primary": "street_4_city",
    "secondary": "street_3_city",
    "tertiary": "street_2_city",
    "residential": "street_1_city",
    "service": "street_service",
    "unclassified": "street_1_city",
    "living_street": "street_1_city",
}

def quote(value: str) -> str:
    return value.replace("\\", "\\\\").replace('"', '\\"')

lines: list[str] = []
written = 0
for tags, refs in ways:
    coords = [nodes[ref] for ref in refs if ref in nodes]
    if len(coords) < 2:
        continue
    highway = tags.get("highway", "")
    item_type = street_type_by_highway.get(highway, "street_1_city")
    label = tags.get("name") or f"SERP demo {highway or 'road'}"
    lines.append(f'type={item_type} label="{quote(label)}"')
    for lat, lon, _tags in coords:
        lines.append(f"{lon} {lat}")
    lines.append("")
    written += 1
    if written >= 350:
        break

poi_count = 0
for lat, lon, tags in nodes.values():
    label = tags.get("name")
    amenity = tags.get("amenity")
    place = tags.get("place")
    if not label or not (amenity or place):
        continue
    item_type = "town_label_2e5" if place else "poi_custom1"
    lines.append(f'{lon} {lat} type={item_type} label="{quote(label)}"')
    poi_count += 1
    if poi_count >= 80:
        break

if written == 0:
    raise SystemExit("No routable ways found in OSM XML; cannot create Navit textfile map")

target.write_text("\n".join(lines) + "\n", encoding="utf-8")
print(f"Wrote Navit textfile map with {written} ways and {poi_count} POIs: {target}")
PY
}

echo "Downloading OSM XML from Overpass..."
echo "Endpoint: $OVERPASS_URL"
echo "Area: $AREA"
echo "BBox: $BBOX"
echo "Output OSM: $OSM_FILE"

rm -f "$OSM_FILE" "$NORMALIZED_OSM"

curl --fail --location --retry 3 --retry-delay 5 \
  -H "Content-Type: application/x-www-form-urlencoded; charset=UTF-8" \
  -H "Accept: */*" \
  --data-urlencode "data=$(cat "$QUERY_FILE")" \
  "$OVERPASS_URL" \
  -o "$OSM_FILE"

if [[ ! -s "$OSM_FILE" ]]; then
  echo "ERROR: downloaded OSM file is empty: $OSM_FILE" >&2
  exit 1
fi

echo "Downloaded:"
ls -lh "$OSM_FILE"

echo "Checking XML..."
xmllint --noout "$OSM_FILE"

echo "Normalizing XML for Navit maptool..."
xmllint --noblanks "$OSM_FILE" > "$NORMALIZED_OSM"

if [[ ! -s "$NORMALIZED_OSM" ]]; then
  echo "ERROR: normalized OSM file is empty: $NORMALIZED_OSM" >&2
  exit 1
fi

echo "Converting OSM XML to Navit bin..."
TMP_BIN="$BIN_FILE.tmp"
rm -f "$TMP_BIN"

if ! maptool "$TMP_BIN" < "$NORMALIZED_OSM" > "$MAPTOOL_LOG" 2>&1; then
  echo "ERROR: maptool failed; log: $MAPTOOL_LOG" >&2
  tail -80 "$MAPTOOL_LOG" >&2
  exit 1
fi

if [[ ! -s "$TMP_BIN" ]]; then
  echo "ERROR: maptool did not create a valid bin file: $TMP_BIN" >&2
  exit 1
fi

mv "$TMP_BIN" "$BIN_FILE"

echo "Creating Navit textfile fallback map..."
write_textfile_map "$NORMALIZED_OSM" "$TEXT_FILE"

echo "Done."
ls -lh "$BIN_FILE"
ls -lh "$TEXT_FILE"
if navit_bin_has_tiles "$BIN_FILE"; then
  echo "Navit bin map contains tile data."
else
  echo "WARNING: Navit bin map contains only index data on this host." >&2
  echo "Navit will use the generated textfile map fallback: $TEXT_FILE" >&2
  echo "maptool log: $MAPTOOL_LOG" >&2
fi
echo "Navit map ready: $BIN_FILE"
echo "Navit textfile fallback ready: $TEXT_FILE"
