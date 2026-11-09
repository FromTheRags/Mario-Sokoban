# Third-party and generated asset notices

## SDL 1.2 runtime

The SDL 1.2 source is vendored in `third_party/SDL-1.2` from upstream commit
`81490298a02fe6806c05ac7b5afde7d88fcc7bf9`. SDL 1.2 is licensed under GNU
LGPL 2.1; its license text is included in `SDL-1.2-LGPL-2.1.txt`. Windows
builds compile that source into a replaceable `SDL.dll`; Linux CI dynamically
links SDL 1.2 and bundles its runtime beside the executable.

The game does not use SDL_image or SDL_ttf. It loads generated BMP artwork
with SDL 1.2 (the transparent player PNGs are decoded by the Windows Imaging
Component on Windows) and draws text with an original 5x7 bitmap typeface
defined in `bitmap_font.hpp`.

## Game artwork

All images in `sprites_mario_sokoban/`, plus `caisse.bmp` and `caisse.ico`,
were newly drawn from geometric primitives by `tools/generate_assets.py` for
this project and are dedicated to the public domain under CC0 1.0. They do not
use extracted game characters, logos, screenshots, or source textures.
