#!/usr/bin/env bash
set -euo pipefail

package_dir="${1:?usage: package_linux.sh PACKAGE_DIR}"
binary="$package_dir/mario-sokoban"
mkdir -p "$package_dir/lib"

# Bundle the LGPL SDL 1.2 runtime and its image/font add-ons. System C runtime
# libraries are intentionally left to the host distribution.
ldd "$binary" | awk '/=> \/.*\/lib(SDL|SDL_image|SDL_ttf|freetype|png|jpeg|tiff|webp|z)[^ ]*\.so/ { print $3 }' | sort -u | while read -r library; do
    [ -n "$library" ] || continue
    cp -L "$library" "$package_dir/lib/"
done
patchelf --set-rpath '$ORIGIN/lib' "$binary"
cp LICENSES.md NotoSans-OFL.txt "$package_dir/"
