#!/usr/bin/env bash
set -euo pipefail

package_dir="${1:?usage: package_linux.sh PACKAGE_DIR}"
binary="$package_dir/mario-sokoban"
mkdir -p "$package_dir/lib"

# Copies the non-glibc shared dependencies of $1 into the package. The host
# distribution remains responsible for the GNU C runtime.
bundle_dependencies() {
    ldd "$1" | awk '$2 == "=>" && $3 ~ /^\// && $1 !~ /^lib(c|m|dl|pthread|rt|resolv)\.so/ && $1 !~ /^ld-linux/ { print $3 }' | sort -u | while read -r library; do
        [ -n "$library" ] || continue
        cp -L "$library" "$package_dir/lib/"
    done
}

bundle_dependencies "$binary"

# Recent distributions (Ubuntu 24.04+, Debian 13+) provide SDL 1.2 through
# sdl12-compat, which loads SDL 2 at run time instead of linking it: bundle
# SDL 2 and its dependencies too, so the game runs where SDL 2 is missing.
sdl12="$(ldd "$binary" | awk '$1 ~ /^libSDL-1\.2\.so/ { print $3 }')"
if [ -n "$sdl12" ] && grep -aq 'libSDL2-2.0.so.0' "$sdl12"; then
    sdl2="$(ldconfig -p | awk '$1 == "libSDL2-2.0.so.0" && /x86-64/ { print $NF; exit }')"
    if [ -z "$sdl2" ]; then
        echo "sdl12-compat needs libSDL2-2.0.so.0, which was not found" >&2
        exit 1
    fi
    cp -L "$sdl2" "$package_dir/lib/"
    bundle_dependencies "$sdl2"
fi

cp LICENSES.md SDL-1.2-LGPL-2.1.txt "$package_dir/"
cat > "$package_dir/run.sh" <<'RUN'
#!/usr/bin/env bash
set -euo pipefail
here="$(cd -- "$(dirname -- "$0")" && pwd)"
export LD_LIBRARY_PATH="$here/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
cd "$here"
exec "$here/mario-sokoban" "$@"
RUN
chmod +x "$package_dir/run.sh"
