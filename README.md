# Mario Sokoban

This repository contains the original SDL 1.2 game and a separate SDL 3 port
on the `SDL_3` branch. CI builds the selected branch and uploads a runnable
Linux artifact. Tag a release as `v*` on the SDL 1.2 branch or `sdl3-v*` on
`SDL_3` to publish the matching package as a GitHub release. Extract the
`.tar.gz` artifact, enter its `package` folder, and start `./run.sh`.

## Build the SDL 1.2 version in CLion

The complete upstream SDL 1.2 source is included in `third_party/SDL-1.2`.
On Windows, CMake builds it with CLion's bundled MinGW toolchain. No
`pkg-config`, SDL installer, SDL_image, or SDL_ttf package is required. Open
the repository as a CMake project, select the `mario-sokoban` target, then
Build and Run. The game loads BMP assets directly and renders text with the
original 5x7 bitmap typeface in `bitmap_font.hpp`.

## Build the SDL 1.2 version on Linux

On Ubuntu 22.04, install CMake, a C++ compiler, and the SDL 1.2 development
package, then run:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
./tools/package_linux.sh build/package
```

Run `build/package/run.sh`. The package contains the SDL 1.2 runtime beside
the game and remains dynamically linked so the library can be replaced, in
keeping with LGPL 2.1. Asset generation and licensing details are in
[LICENSES.md](LICENSES.md).
