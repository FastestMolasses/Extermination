set -e
cd /work
ASM=build/asm/matchings/main/code
for f in func_0010CD58 func_0010AA80 func_0010CE28 func_00101BE0 func_00107098; do
  d=tools/permuter/work/ee_$f
  echo "============================================================"
  echo "=== PERMUTE $f ==="
  # assemble target
  mipsel-linux-gnu-as -march=r5900 config/asm_prelude.inc build/macro.inc "$ASM/$f.s" -o "$d/target.o" 2>/dev/null || { echo "  as FAIL"; continue; }
  rm -rf "$d"/output-* 2>/dev/null || true
  # sanity: does base.c compile + score?
  ( cd tools/permuter && timeout 420 python3 permuter.py "work/ee_$f" --stop-on-zero --best-only -j6 2>&1 | grep -viE "Usage Warning|MWCIncludes|DeprecationWarning|warnings.warn" | tail -25 )
  echo "  outputs: $(ls -d $d/output-* 2>/dev/null || echo none)"
done
echo "ALL_DONE"
