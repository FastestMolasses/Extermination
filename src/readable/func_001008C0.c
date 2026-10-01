// NEARMISS func_001008C0  (vram 0x001008C0, 0x104 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 47.63% via ee-gcc 2.9-991111-01 (-O2) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// ee-gcc scheduling of the stores and 64-bit packing; the original also ends with a sync
// instruction (no C form).
//
// The function links from the asm body in src/func_001008C0.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: eegcc
// CFLAGS: -O2

// SDK libgraph: fills a default clear packet (a sprite covering the screen
// with the depth test forced, returns 6 = the quadword count). (x, y) / (w,
// h) are the area, (r, g, b, a) the colour, z the depth, ztest the test.
//  TEST_1 (0x47) = 0x30000 (always pass), PRIM (0x00) = 6 (sprite),
//  RGBAQ (0x01) = the colour with Q = 1.0, XYZ2 (0x05) the two corners
//  (x * 16, y * 16, z) and ((x + w) * 16, (y + h) * 16, z), then TEST_1
//  restored: ZTE with ZTST = ztest when non-zero, else always pass.
// The original ends with a sync instruction (no C form).
int func_001008C0(long long *p, short ztest, short x, short y, short w, short h,
                  unsigned char r, unsigned char g, unsigned char b, unsigned char a, unsigned int z) {
    long long zz = (long long)z << 32;

    p[2] = 6;
    p[5] = 1;
    p[4] = (r & 0xFF) | 0x3F80000000000000LL | ((long long)b << 16) | ((g & 0xFF) << 8) | ((long long)a << 24);
    p[6] = (x * 16) | ((y * 16) << 16) | zz;
    p[9] = 5;
    p[8] = ((x + w) * 16) | (((y + h) * 16) << 16) | zz;
    p[11] = 0x47;
    p[1] = 0x47;
    p[0] = 0x30000;
    p[3] = 0;
    p[7] = 5;
    if (ztest != 0) {
        p[10] = ((ztest & 3) << 17) | 0x10000;
    } else {
        p[10] = 0x30000;
    }
    return 6;
}
