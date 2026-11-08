# Mario Sokoban

This repository contains the original SDL 1.2 game and a separate SDL 3 port
on the `SDL_3` branch. CI builds the selected branch and uploads a runnable
Linux artifact. Tag a release as `v*` to publish the SDL 1.2 package as a
GitHub release. Extract the `.tar.gz` artifact and start it with `./run.sh`.

## Build the SDL 1.2 version

On Ubuntu 22.04, install CMake, a C++ compiler, pkg-config, SDL 1.2, SDL_image,
and SDL_ttf development packages, then run:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
./tools/package_linux.sh build/package
```

Run `build/package/run.sh`. The package contains the SDL 1.2 runtime beside
the game and remains dynamically linked so the library can be replaced, in
keeping with LGPL 2.1. Asset generation and licensing details are in
[LICENSES.md](LICENSES.md).
