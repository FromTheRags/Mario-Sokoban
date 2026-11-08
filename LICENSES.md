# Third-party and generated asset notices

## SDL 1.2 runtime

The legacy build dynamically links against the upstream SDL 1.2 runtime and
SDL_image / SDL_ttf add-ons supplied by the build runner. The Linux CI artifact
bundles the SDL shared libraries beside the executable. SDL 1.2 is licensed
under GNU LGPL 2.1; see the upstream [SDL 1.2 source and license](https://github.com/libsdl-org/SDL-1.2).
The license text is included in `SDL-1.2-LGPL-2.1.txt`, and users can replace
the shared library in the Linux artifact.

SDL_image and SDL_ttf are distributed under the zlib license. Their sources
and notices are available from [SDL_image](https://github.com/libsdl-org/SDL_image)
and [SDL_ttf](https://github.com/libsdl-org/SDL_ttf); their license texts are
included as `SDL-image-zlib.txt` and `SDL-ttf-zlib.txt`.

## SDL 3 port

The `SDL_3` branch dynamically links against SDL 3.4.20, SDL3_image, and
SDL3_ttf. SDL 3 and its satellite libraries use the zlib license; the Linux CI
artifact bundles the runtime libraries beside the executable.

## Noto Sans

The bundled font data is Noto Sans from the Google Fonts project, licensed
under the SIL Open Font License 1.1. See `NotoSans-OFL.txt` and
[the upstream font directory](https://github.com/google/fonts/tree/main/ofl/notosans).

## Game artwork

All images in `sprites_mario_sokoban/`, plus `caisse.bmp` and `caisse.ico`,
were newly drawn from geometric primitives by `tools/generate_assets.py` for
this project and are dedicated to the public domain under CC0 1.0. They do not
use extracted game characters, logos, screenshots, or source textures.
The CC0 1.0 legal code is available from [Creative Commons](https://creativecommons.org/publicdomain/zero/1.0/).
