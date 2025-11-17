# Building Snake++ for WebAssembly

This guide explains how to build Snake++ for WebAssembly using Emscripten.

## Prerequisites

1. **Install Emscripten SDK**
   - Visit https://emscripten.org/docs/getting_started/downloads.html
   - Follow the installation instructions for your platform
   - Make sure `emcc` is in your PATH

2. **Verify Installation**
   ```bash
   emcc --version
   ```

## Building

### Method 1: Using the Build Script (Recommended)

Simply run the build script:

```bash
./build-wasm.sh
```

This will:
- Compile all C++ sources to WebAssembly
- Bundle SDL2 libraries (SDL2, SDL2_ttf, SDL2_mixer, SDL2_image)
- Preload all game assets (fonts, sounds, images)
- Generate `build-wasm/snake++.html`

### Method 2: Using CMake

If you prefer using CMake:

```bash
mkdir -p build-wasm
cd build-wasm
emcmake cmake -f ../CMakeLists.emscripten.txt ..
emmake make
```

## Running

**Important**: Due to browser security restrictions, you must serve the files through a web server. You cannot simply open the HTML file directly.

### Option 1: Python HTTP Server

```bash
cd build-wasm
python3 -m http.server 8000
```

Then open http://localhost:8000/snake++.html in your browser.

### Option 2: Node.js HTTP Server

```bash
cd build-wasm
npx http-server -p 8000
```

Then open http://localhost:8000/snake++.html in your browser.

### Option 3: Any Other Web Server

You can use any web server (Apache, Nginx, etc.) to serve the `build-wasm` directory.

## Controls

The game controls remain the same:
- **WASD** or **Arrow Keys**: Change snake direction
- **ESC** or **P**: Pause/Resume game
- **M**: Mute/Unmute sounds

## Technical Details

### Build Configuration

- **Memory**: Initial 64MB, grows as needed
- **Optimization**: O2 level
- **Asyncify**: Enabled for async file operations
- **WebGL**: WebGL2 with full ES3 support

### File System

The game uses Emscripten's virtual file system. Assets are preloaded at build time. Save data is stored in the virtual file system but will not persist between browser sessions by default.

To enable persistent save data, you would need to:
1. Mount IDBFS (IndexedDB File System) in your code
2. Sync the file system before and after save operations

### Assets

All assets from the `assets/` directory are preloaded into the WASM file system at `/assets/`. This includes:
- Fonts: `assets/fonts/pixel-letters.ttf`
- Sounds: `assets/sounds/*.wav`
- Images: `assets/icons/*.png`

## Troubleshooting

### "emcc not found"
- Make sure Emscripten SDK is installed and activated
- Run `source emsdk_env.sh` (or equivalent) in your terminal

### "CORS error" or assets not loading
- Make sure you're serving through a web server, not opening the file directly
- Check that the web server is running and accessible

### Game runs slowly
- Try building with `-O3` instead of `-O2` in the build script
- Check browser console for performance warnings

### Save data not persisting
- This is expected behavior - the default file system is in-memory only
- To enable persistence, implement IDBFS mounting (see Technical Details above)

## Deployment

To deploy to a web server:
1. Build the project using the build script
2. Upload the entire `build-wasm` directory to your web server
3. Ensure the web server serves the files with correct MIME types
4. Access `snake++.html` through your web server

