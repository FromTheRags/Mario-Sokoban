# Mario Sokoban

This repository contains the original SDL 1.2 game and a separate SDL 3 port
on the `SDL_3` branch. CI builds the selected branch and uploads a runnable
Linux artifact. Tag a release as `v*` to publish the build artifacts as a
GitHub release. Launch the artifact with `./run.sh` from the extracted folder.

The SDL 3 branch targets SDL 3.4.20, SDL3_image 3.4.8, and SDL3_ttf 3.2.2.
`legacy_sdl3.hpp` adapts the old screen-surface and event calls to SDL 3 while
preserving the existing menus, editor, levels, and Sokoban rules.

## Build the SDL 1.2 version

On Ubuntu 22.04, install CMake, a C++ compiler, pkg-config, SDL 1.2, SDL_image,
and SDL_ttf development packages, then run:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
./tools/package_linux.sh build/package
```

Run `build/package/mario-sokoban` with `build/package` as the working
directory. The packaged SDL 1.2 runtime remains dynamically linked and can be
replaced, in keeping with LGPL 2.1. Asset generation and licensing details are
in [LICENSES.md](LICENSES.md).
