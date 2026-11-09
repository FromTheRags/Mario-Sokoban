# Mario Sokoban

A Sokoban game in C++ with SDL 1.2 (this branch, `master`). The `SDL_3` branch
contains an SDL 3 port with the same screens, graphics and texts.

![Gameplay](docs/gameplay.gif)

| Main menu | Level selection |
| --- | --- |
| ![Main menu](docs/menu.png) | ![Level selection](docs/levels.png) |
| **Personal levels (3 pages of 9)** | **Level editor** |
| ![Personal levels](docs/custom.png) | ![Level editor](docs/editor.png) |

## Downloads and continuous integration

The GitHub Actions workflow (`.github/workflows/build.yml`) builds the game on
every push:

- **Linux** (Ubuntu 22.04): `mario-sokoban-linux.tar.gz`. Extract it, enter
  the `package` folder and start `./run.sh`.
- **Windows** (MSYS2 MinGW-w64): `mario-sokoban-windows.zip`, a portable
  folder (start `mario-sokoban.exe`), and `setup_mario_sokoban-<version>.exe`,
  an installer built with Inno Setup from `installer/mario-sokoban.iss`.

The files are available as artifacts of each workflow run. Pushing a tag
`v<version>` (for example `v1.6`) also publishes them as a GitHub release, and
the version number is used by the installer.

## Playing

- Six built-in levels, with a top 3 of the best times for each level.
- A level editor (`2 - CREER`): `MODIFIER` opens a personal level slot (an
  empty slot starts a new level), `REINITIALISATION` empties a slot and clears
  its medal and best times. `GO` test-plays the level being edited without
  recording a time, and the last toolbar button reloads the last saved version
  of the level.
- Personal levels are organised in **3 pages of 9 levels** (27 levels),
  always available. Switch pages with the `< PREC` / `SUIV >` buttons or the
  Left/Right (Page Up/Page Down) keys. Levels are stored in
  `niveaux perso/niveau1.txt` ... `niveau27.txt`; best times and medals in
  `data/`.
- Press **F11** (or **Alt+Enter**) to toggle fullscreen, **Esc** leaves
  fullscreen, or start the game with `--fullscreen` (`-f`). The 680x544 game
  is scaled to the screen, keeps its aspect ratio and is centred with black
  borders. The window can also be resized freely.

## Build the SDL 1.2 version on Windows

The complete upstream SDL 1.2 source is included in `third_party/SDL-1.2` and
is compiled together with the game, so no SDL installer, `pkg-config`,
SDL_image or SDL_ttf package is required. PNG sprites are loaded with the
native Windows Imaging Component and text is drawn with the built-in 5x7
bitmap typeface (`bitmap_font.hpp`).

Requirements: CMake 3.20 or newer and a **MinGW-w64 GCC** toolchain (the one
bundled with CLion, MSYS2, or a standalone MinGW-w64). Visual Studio (MSVC)
is not a supported toolchain for this project.

### With CLion

1. *File > Open* the repository folder and open it as a CMake project.
2. Keep the default toolchain (CLion's bundled MinGW) in
   *Settings > Build, Execution, Deployment > Toolchains*.
3. Select the `mario-sokoban` run configuration, then *Build* and *Run*.

The executable is produced in `cmake-build-<profile>/package/` next to
`SDL.dll`, `libwinpthread-1.dll` and a copy of every asset folder. The game
switches to the executable's folder at startup if its assets are not found in
the working directory, so no working-directory setting is needed.

### From a terminal (MinGW-w64)

Make sure `gcc`, `g++`, `mingw32-make` (or `ninja`) and `cmake` are on the
`PATH`, then from the repository folder:

```bat
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
build\package\mario-sokoban.exe
```

With MSYS2, run the same commands from the *MINGW64* shell (use
`-G "MSYS Makefiles"` or `-G Ninja` if you prefer).

To distribute the game, copy the whole `build\package` folder: it contains
the executable (with `libgcc`/`libstdc++` linked statically), `SDL.dll`,
`libwinpthread-1.dll`, the sprites, the levels, `data` and the licence files.
The Windows executable icon is generated from the player sprite.

### Windows installer

Install [Inno Setup](https://jrsoftware.org/isinfo.php) 6 or 7, build the
game in Release into `build` (commands above), then run:

```bat
iscc installer\mario-sokoban.iss
```

The installer is written to `build\installer`. Pass `/DAppVersion=1.6` to set
the version, or `/DSourceDir=<folder>` to package another build folder (an
absolute path, or relative to `installer`, for example
`..\cmake-build-release\package`). It installs into *Program Files*,
lets every user write the `data` and `niveaux perso` folders (best times,
medals and personal levels) and never overwrites existing personal levels.

## Build the SDL 1.2 version on Linux

On Ubuntu 22.04, install CMake, a C++ compiler, and the SDL 1.2 development
package, then run:

```sh
sudo apt-get install cmake g++ libsdl1.2-dev
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
./tools/package_linux.sh build/package
```

Run `build/package/run.sh`. The package contains the SDL 1.2 runtime beside
the game and remains dynamically linked so the library can be replaced, in
keeping with LGPL 2.1.

## Assets and licences

All images are original pixel art generated by `tools/generate_assets.py`
(CC0 1.0); text uses the original 5x7 bitmap typeface of `bitmap_font.hpp`.
Licensing details are in [LICENSES.md](LICENSES.md).
