#!/bin/bash
# run_func233.sh <func> <sdatathreshold> [timeout_s] [jobs] [compiler]
# Like run_func.sh but: (a) compiles with mwcc 2.3.3 (tools/mwccps2-233) by default
# — most regalloc/branch near-misses are best on 2.3.3 — and (b) writes a TUNED
# settings.toml that maxes the randomizers which actually move register-allocation
# order / scheduling state and zeroes the cosmetic ones (per s84 permuter research).
# Pass a 5th arg to override the compiler exe (e.g. tools/mwccps2/mwccmips.exe).
# Assumes tools/permuter/work/<func>/base.c exists (PLAIN, pycparser-parseable C).
set -e
FN="$1"; SDT="${2:-0}"; TMO="${3:-1200}"; JOBS="${4:-3}"; CCEXE="${5:-tools/mwccps2-233/mwccps2.exe}"
D="tools/permuter/work/$FN"
[ -f "$D/base.c" ] || { echo "ERROR: $D/base.c missing"; exit 1; }
cat > "$D/settings.toml" <<TOML
func_name = "$FN"
compiler_type = "mwcc"
[weight_overrides]
# state-changing randomizers (the regalloc/schedule levers) — maxed
perm_temp_for_expr = 120
perm_reorder_decls = 90
perm_reorder_stmts = 60
perm_split_assignment = 40
perm_add_self_assignment = 40
perm_commutative = 30
perm_add_sub = 20
perm_expand_expr = 30
perm_ins_block = 20
perm_condition = 15
# cosmetic randomizers (waste cycles for reg-perms) — zeroed
perm_sameline = 0
perm_struct_ref = 0
perm_cast_simple = 0
perm_dummy_comma_expr = 0
perm_inequalities = 0
perm_empty_stmt = 0
perm_refer_to_var = 0
TOML
cat > "$D/compile.sh" <<SH
#!/bin/bash
cd /work
qemu-i386 tools/bin/wibo32 $CCEXE -c -O4,p -sdatathreshold $SDT -o "\$3" "\$1" 2>/dev/null
SH
chmod +x "$D/compile.sh"
# Assemble the target from the NORMALIZED source (build/.asmnorm/<F>.s), not the raw
# splat .s. The normalized file carries the VU0 $ACC/$Q and invented-symbol fixups AND
# the function's own jump-table rodata. Using the raw .s leaves jtbl_XXXX
# undefined-external, so for every jr-table function the dispatch relocation is a
# permanent residual the permuter can never fix — it would burn a full budget chasing an
# artifact of its own setup. (s85: caught by an agent on func_001A9C40.) Regenerate the
# normalized file if it is missing or stale.
.venv/bin/python3 -c "import sys;sys.path.insert(0,'tools/decomp');import build;build.normalize_asm('$FN')" 2>/dev/null
SRC_S="build/.asmnorm/$FN.s"
[ -f "$SRC_S" ] || SRC_S="build/asm/matchings/main/code/$FN.s"
mipsel-linux-gnu-as -march=r5900 config/asm_prelude.inc build/macro.inc \
    "$SRC_S" -o "$D/target.o" 2>/dev/null
rm -rf "$D"/output-* 2>/dev/null || true
cd tools/permuter
echo "[run_func233] permuting $FN (cc=$CCEXE sdt=$SDT ${TMO}s -j$JOBS, tuned weights) ..."
timeout "$TMO" python3 permuter.py "work/$FN" --stop-on-zero --best-only -j "$JOBS" 2>&1 | grep -vE "Usage Warning|MWCIncludes" | tail -25
echo "[run_func233] outputs:"; ls -d "work/$FN"/output-0-* 2>/dev/null || echo "  (no score-0)"
