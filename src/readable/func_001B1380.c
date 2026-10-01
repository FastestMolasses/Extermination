// NEARMISS func_001B1380  (vram 0x001B1380, 0x68 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 88.46% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// Boolean lowering of the final compare: the original branches to the exit with 0 in the delay
// slot, loads 1 on the fall-through and leaves a dead 0-load; mwcc emits the inverted short form
// (if/else, ternary, result-variable and negated spellings measured).
//
// The function links from the asm body in src/func_001B1380.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Side test: is the heading from b to a (atan2f(a.x - b.x, a.z - b.z),
// func_0011E620) at or to the left of heading, after wrapping the difference
// into (-pi, pi] (func_001B1470)? Returns 1 when the wrapped difference is
// >= 0, else 0.
extern float func_0011E620(float y, float x);
extern float func_001B1470(float angle);

int func_001B1380(float *a, float *b, float heading) {
    return !(func_001B1470(func_0011E620(a[0] - b[0], a[2] - b[2]) - heading) < 0.0f);
}
