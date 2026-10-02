// NEARMISS func_0019F680  (vram 0x0019F680, 0xA8 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 97.10% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// the original loads obj+0x1C before the table base 0x70003204
// and loads the element index into v1 for the y / z reads; mwcc 2.3.3 loads the base
// first and uses a2 (operand order, volatile and local spellings measured; the
// canonicalised add order ignores the source order).
//
// The function links from the asm body in src/func_0019F680.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. Later-level decomp lane (LDEC) 2026-10-01 (docs/LEVELS_DECOMP.md).
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Reads vertex i of obj into out: returns 0 when i >= obj+0x18 (byte count);
// otherwise the halfword index at (0x70003204 base + obj+0x1C) + 2i selects a 12-byte
// entry of the table at 0x700031FC whose three floats are copied to out; returns the
// count.
int func_0019F680(float *out, unsigned char *obj, int i) {
    char *base = *(char **)0x70003204 + *(int *)(obj + 0x1C);
    short *idx;
    if (i >= obj[0x18]) {
        return 0;
    }
    idx = (short *)base + i;
    out[0] = *(float *)(*(char **)0x700031FC + *idx * 12);
    out[1] = *(float *)(*(char **)0x700031FC + *idx * 12 + 4);
    out[2] = *(float *)(*(char **)0x700031FC + *idx * 12 + 8);
    return obj[0x18];
}
