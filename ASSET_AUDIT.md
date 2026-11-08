# Asset audit

The original 2013 project included uncredited Mario character sprites, a dragon
image, several photographic or textured backgrounds, editor icons, Photoshop
PXDs, and six typefaces with no license information in the repository. Their
source and permissions could not be established from the files provided, so
the old artwork and fonts were not carried into the source history.

- All images used by the game were regenerated as original garden-themed
  pixel art by `tools/generate_assets.py`; they are dedicated under CC0 1.0.
- The SDL 1.2 build uses an original 5x7 bitmap typeface in `bitmap_font.hpp`;
  the SDL3 branch uses the included OFL-licensed Noto Sans font.
- The original Photoshop files and unused icon were excluded.
- The source code and the product name were not rewritten as part of this
  resource audit.

This is a repository-level provenance review of supplied visual resources;
it does not establish the authorship or licensing of the original source code
or name.
