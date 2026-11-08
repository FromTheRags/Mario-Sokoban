# SDL 1.2 source

This directory contains the upstream `libsdl-org/SDL-1.2` repository at
commit `81490298a02fe6806c05ac7b5afde7d88fcc7bf9` (retrieved 2026-11-08).
The included `COPYING` file contains the upstream LGPL 2.1 license.

The Windows CMake build reads its source list from
`VisualC/SDL/SDL.vcxproj`, then builds the listed C files with the selected
compiler. The upstream MSVC-only MMX file is omitted.
