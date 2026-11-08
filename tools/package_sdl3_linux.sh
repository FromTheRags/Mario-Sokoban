#!/usr/bin/env bash
set -euo pipefail

package_dir="${1:?usage: package_sdl3_linux.sh PACKAGE_DIR}"
binary="$package_dir/mario-sokoban"
mkdir -p "$package_dir/lib"
ldd "$binary" | awk '$2 == "=>" && $3 ~ /^\// && $1 !~ /^lib(c|m|dl|pthread|rt|resolv)\.so/ && $1 !~ /^ld-linux/ { print $3 }' | sort -u | while read -r library; do
    [ -n "$library" ] || continue
    cp -L "$library" "$package_dir/lib/"
done
cat > "$package_dir/run.sh" <<'RUN'
#!/usr/bin/env bash
set -euo pipefail
here="$(cd -- "$(dirname -- "$0")" && pwd)"
export LD_LIBRARY_PATH="$here/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
cd "$here"
exec "$here/mario-sokoban" "$@"
RUN
chmod +x "$package_dir/run.sh"
