# Third-party and generated asset notices

## SDL 1.2 runtime

The SDL 1.2 source is vendored in `third_party/SDL-1.2` from upstream commit
`81490298a02fe6806c05ac7b5afde7d88fcc7bf9`. SDL 1.2 is licensed under GNU
LGPL 2.1; its license text is included in `SDL-1.2-LGPL-2.1.txt`. Windows
builds compile that source into a replaceable `SDL.dll`; Linux CI dynamically
links SDL 1.2 and bundles its runtime beside the executable.

The SDL 1.2 branch no longer links SDL_image or SDL_ttf. It loads generated
BMP artwork with SDL 1.2 and draws text with an original 5x7 bitmap typeface.
The SDL3 branch uses SDL3_image and SDL3_ttf; their zlib notices are included
as `SDL-image-zlib.txt` and `SDL-ttf-zlib.txt`.

## Noto Sans

The bundled font data is Noto Sans from the Google Fonts project, licensed
under the SIL Open Font License 1.1. See `NotoSans-OFL.txt` and
[the upstream font directory](https://github.com/google/fonts/tree/main/ofl/notosans).

## Game artwork

All images in `sprites_mario_sokoban/`, plus `caisse.bmp` and `caisse.ico`,
were newly drawn from geometric primitives by `tools/generate_assets.py` for
this project and are dedicated to the public domain under CC0 1.0. They do not
use extracted game characters, logos, screenshots, or source textures.
