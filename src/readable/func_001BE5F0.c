// NEARMISS func_001BE5F0  (vram 0x001BE5F0, 0xC4 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 92.35% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// the original ends with an if / else whose dead join-head copy
// (return value 1) follows the last unconditional branch, and holds the zone pointer
// in v0; mwcc 2.3.3 gives the same compare without the dead copy and uses v1 (else-if,
// ternary, inverted and result-variable spellings measured).
//
// The function links from the asm body in src/func_001BE5F0.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. Later-level decomp lane (LDEC) 2026-10-01 (docs/LEVELS_DECOMP.md).
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Zone test: 0 when the scratchpad byte 0x70003B8D is nonzero; otherwise 1 when the
// horizontal distance sqrt(dx^2 + dz^2) (func_0011E748) between a+0xA0/+0xA8 and
// b+0xB0/+0xB8 is at most zone->0x18[0] and |a+0xA4 - b+0xB4| (func_0011DF78) is at
// most zone->0x18[1], else 0.
extern unsigned char D_70003B8D;
extern float func_0011E748(float x);
extern float func_0011DF78(float x);

int func_001BE5F0(char *a, char *b, char *zone) {
    float dx;
    float dz;
    if (D_70003B8D != 0) {
        return 0;
    }
    dx = *(float *)(a + 0xA0) - *(float *)(b + 0xB0);
    dz = *(float *)(a + 0xA8) - *(float *)(b + 0xB8);
    if (!(func_0011E748(dx * dx + dz * dz) <= *(float *)(*(char **)(zone + 0x18) + 0))) {
        return 0;
    } else {
        return func_0011DF78(*(float *)(a + 0xA4) - *(float *)(b + 0xB4)) <= *(float *)(*(char **)(zone + 0x18) + 4);
    }
}
