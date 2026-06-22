#!/usr/bin/env bash
set -euo pipefail

if [[ "$(uname)" == "Darwin" ]]; then
  brew install gpsd curl libxml2 python3
  echo "macOS: navit and maptool are not available in Homebrew."
  echo "Build navit from source or use Linux for full Navit integration."
else
  if [[ "${EUID:-$(id -u)}" -ne 0 ]]; then
    SUDO=${SUDO:-sudo}
  else
    SUDO=${SUDO:-}
  fi

  export DEBIAN_FRONTEND=${DEBIAN_FRONTEND:-noninteractive}
  $SUDO apt-get update
  $SUDO apt-get install -y \
    navit \
    navit-data \
    navit-graphics-gtk-drawing-area \
    navit-gui-gtk \
    navit-gui-internal \
    maptool \
    gpsd \
    gpsd-clients \
    curl \
    libxml2-utils \
    ca-certificates \
    python3
fi

cat <<'EOF'
SERP Navit dependencies installed.
Next:
  scripts/download_navit_map.sh
  scripts/check_navit_setup.sh
  gen/integrations/navit/start_navit_demo.sh
EOF
