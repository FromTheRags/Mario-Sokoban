#!/usr/bin/env bash
set -euo pipefail

package_dir="${1:?usage: package_linux.sh PACKAGE_DIR}"
binary="$package_dir/mario-sokoban"
mkdir -p "$package_dir/lib"

# Bundle non-glibc runtime libraries, including SDL 1.2 and its image/font
# add-ons. The GNU C runtime remains supplied by the host distribution.
ldd "$binary" | awk '$2 == "=>" && $3 ~ /^\// && $1 !~ /^lib(c|m|dl|pthread|rt|resolv)\.so/ && $1 !~ /^ld-linux/ { print $3 }' | sort -u | while read -r library; do
    [ -n "$library" ] || continue
    cp -L "$library" "$package_dir/lib/"
done
cp LICENSES.md NotoSans-OFL.txt SDL-1.2-LGPL-2.1.txt SDL-image-zlib.txt SDL-ttf-zlib.txt "$package_dir/"
cat > "$package_dir/run.sh" <<'RUN'
#!/usr/bin/env bash
set -euo pipefail
here="$(cd -- "$(dirname -- "$0")" && pwd)"
export LD_LIBRARY_PATH="$here/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
cd "$here"
exec "$here/mario-sokoban" "$@"
RUN
chmod +x "$package_dir/run.sh"
