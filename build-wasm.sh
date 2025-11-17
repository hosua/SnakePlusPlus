#!/bin/bash

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/build-wasm"
ASSETS_DIR="${SCRIPT_DIR}/assets"
SOURCES_DIR="${SCRIPT_DIR}/src"

echo "Building Snake++ for WebAssembly with Emscripten..."

if ! command -v emcc &> /dev/null; then
    echo "Error: emcc not found. Please install Emscripten SDK."
    echo "Visit: https://emscripten.org/docs/getting_started/downloads.html"
    exit 1
fi

mkdir -p "${BUILD_DIR}"

echo "Compiling sources..."
emcc \
    $(find "${SOURCES_DIR}" -name "*.cc") \
    -I"${SOURCES_DIR}" \
    -std=c++17 \
    -DEMSCRIPTEN \
    -s USE_SDL=2 \
    -s USE_SDL_TTF=2 \
    -s USE_SDL_MIXER=2 \
    -s USE_SDL_IMAGE=2 \
    -s SDL2_IMAGE_FORMATS='["png"]' \
    -s ALLOW_MEMORY_GROWTH=1 \
    -s INITIAL_MEMORY=67108864 \
    -s USE_WEBGL2=1 \
    -s FULL_ES3=1 \
    -O2 \
    --preload-file "${ASSETS_DIR}@/assets" \
    --use-preload-plugins \
    -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap","FS","print","printErr"]' \
    -s EXPORTED_FUNCTIONS='["_main","_malloc","_free"]' \
    -s ASYNCIFY \
    -s ASSERTIONS=1 \
    -lidbfs.js \
    --shell-file "${SCRIPT_DIR}/shell.html" \
    -o "${BUILD_DIR}/snake++.html" \
    -Wall

echo "Copying assets..."
cp -r "${ASSETS_DIR}" "${BUILD_DIR}/" || true

echo ""
echo "Build complete! Output files are in: ${BUILD_DIR}"
echo "Open ${BUILD_DIR}/snake++.html in a web browser to play."
echo ""
echo "To serve locally (required for file system access):"
echo "  cd ${BUILD_DIR}"
echo "  python3 -m http.server 8000"
echo "  Then open http://localhost:8000/snake++.html"

