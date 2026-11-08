# Third-party and generated asset notices

## SDL 1.2 runtime

The legacy build dynamically links against the upstream SDL 1.2 runtime and
SDL_image / SDL_ttf add-ons supplied by the build runner. The Linux CI artifact
bundles the SDL shared libraries beside the executable. SDL 1.2 is licensed
under GNU LGPL 2.1; see the upstream [SDL 1.2 source and license](https://github.com/libsdl-org/SDL-1.2).
The source remains available so users can replace the shared library.

SDL_image and SDL_ttf are distributed under the zlib license. Their sources
and notices are available from [SDL_image](https://github.com/libsdl-org/SDL_image)
and [SDL_ttf](https://github.com/libsdl-org/SDL_ttf).

## Noto Sans

The bundled font data is Noto Sans from the Google Fonts project, licensed
under the SIL Open Font License 1.1. See `NotoSans-OFL.txt` and
[the upstream font directory](https://github.com/google/fonts/tree/main/ofl/notosans).

## Game artwork

All images in `sprites_mario_sokoban/`, plus `caisse.bmp` and `caisse.ico`,
were newly drawn from geometric primitives by `tools/generate_assets.py` for
this project and are dedicated to the public domain under CC0 1.0. They do not
use extracted game characters, logos, screenshots, or source textures.
