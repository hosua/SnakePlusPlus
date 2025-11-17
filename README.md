# Snake++
Snake clone made with C++ and SDL2.

## Building

### Native Build

Standard CMake build:

```bash
mkdir build && cd build
cmake ..
make
```

### WebAssembly Build

Build for the web using Emscripten:

```bash
./build-wasm.sh
```

See [WASM_BUILD.md](WASM_BUILD.md) for detailed instructions.

## Controls

```
WASD/Arrow Keys to change directions.
ESC/P to pause the game.
M to mute/unmute sounds
```

![snake](img/snake-02.gif)

