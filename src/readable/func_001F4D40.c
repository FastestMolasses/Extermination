// NEARMISS func_001F4D40  (vram 0x001F4D40, 0xE0 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 98.93% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// Register allocation of the packed colour (the original builds it in t0..t2; OR order and operand
// grouping measured).
//
// The function links from the asm body in src/func_001F4D40.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Draws a flickering glow sprite at pos. col points at four unsigned words
// (r, g, b, a); one random byte k (bits 23..30 of func_00122BB8()) gives the
// brightness s = (3a + a * k / 256) / 4, and each of r, g, b is scaled by
// s / 128 into a packed 0xBBGGRR colour. func_001CD520(0, 2, pos,
// 0x20045B0599421EF0, size, size, depth, colour) draws it.
extern int func_00122BB8(void);
extern void func_001CD520(int kind, int mode, void *pos, long long tag, float w, float h, float z, unsigned int rgb);

void func_001F4D40(void *pos, unsigned int *col, float size, float depth) {
    unsigned int a = col[3];
    unsigned int s;

    s = (a * 3 + ((a * ((func_00122BB8() >> 23) & 0xFF)) >> 8)) >> 2;
    func_001CD520(0, 2, pos, 0x20045B0599421EF0LL, size, size, depth,
                  ((col[2] * s) >> 7) << 16 | ((col[1] * s) >> 7) << 8 | ((col[0] * s) >> 7));
}
