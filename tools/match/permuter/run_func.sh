#!/bin/bash
# Usage: run_func.sh <func_name> <sdatathreshold> [timeout_s] [jobs]
# Assumes tools/permuter/work/<func>/base.c already exists (your near-matching C).
# Assembles target.o, writes compile.sh + settings.toml, runs the permuter.
# A found 100% match lands in tools/permuter/work/<func>/output-0-*/source.c
set -e
FN="$1"; SDT="${2:-0}"; TMO="${3:-900}"; JOBS="${4:-4}"
D="tools/permuter/work/$FN"
[ -f "$D/base.c" ] || { echo "ERROR: $D/base.c missing (write your near-miss C first)"; exit 1; }
cat > "$D/settings.toml" <<TOML
func_name = "$FN"
compiler_type = "mwcc"
TOML
cat > "$D/compile.sh" <<SH
#!/bin/bash
cd /work
qemu-i386 tools/bin/wibo32 tools/mwccps2/mwccmips.exe -c -O4,p -sdatathreshold $SDT -o "\$3" "\$1" 2>/dev/null
SH
chmod +x "$D/compile.sh"
mipsel-linux-gnu-as -march=r5900 config/asm_prelude.inc build/macro.inc \
    build/asm/matchings/main/code/$FN.s -o "$D/target.o" 2>/dev/null
rm -rf "$D"/output-* 2>/dev/null || true
cd tools/permuter
echo "[run_func] permuting $FN (sdt=$SDT, ${TMO}s, -j$JOBS) ..."
timeout "$TMO" python3 permuter.py "work/$FN" --stop-on-zero --best-only -j "$JOBS" 2>&1 | grep -vE "Usage Warning|MWCIncludes"
echo "[run_func] outputs:"; ls -d "work/$FN"/output-* 2>/dev/null || echo "  (none)"
