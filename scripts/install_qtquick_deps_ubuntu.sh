#!/usr/bin/env bash
set -euo pipefail

if [[ "$(uname)" == "Darwin" ]]; then
  brew install qt5
  echo "macOS: Qt5 installed via Homebrew."
  echo "Note: QML modules are bundled with qt5 formula."
else
  sudo apt-get update
  sudo apt-get install -y \
    qtbase5-dev \
    qtdeclarative5-dev \
    qtquickcontrols2-5-dev \
    qml-module-qtquick2 \
    qml-module-qtquick-controls2 \
    qml-module-qtquick-layouts \
    qml-module-qtquick-window2
fi
