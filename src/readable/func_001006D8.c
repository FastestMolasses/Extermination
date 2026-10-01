// NEARMISS func_001006D8  (vram 0x001006D8, 0x1E4 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 84.05% via ee-gcc 2.9-991111-01 (-O2) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// ee-gcc scheduling of the 64-bit register packing; the original also ends with a sync
// instruction, which has no C form.
//
// The function links from the asm body in src/func_001006D8.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: eegcc
// CFLAGS: -O2

// SDK libgraph: fills a default draw environment (eight A+D register pairs,
// returns 8 = the quadword count). psm is the frame buffer format, w / h the
// size, ztest / zpsm the depth test and depth format.
//  FRAME_1 (0x4C): FBW = (w + 63) / 64, PSM = psm.
//  ZBUF_1 (0x4E): ZBP = the block after the frame buffer (func_00100610),
//    ZPSM = zpsm, ZMSK set when ztest is 0.
//  XYOFFSET_1 (0x18): the 2048-centred origin, (0x800 - w/2) * 16 and
//    (0x800 - h/2) * 16.
//  SCISSOR_1 (0x40): 0..w-1, 0..h-1.
//  PRMODECONT (0x1A) / COLCLAMP (0x46) bit 0 set; DTHE (0x45) bit 0 from
//    psm bit 1 (16-bit formats dither).
//  TEST_1 (0x47): ZTE with ZTST = ztest when ztest is non-zero, else 0x30000
//    (always pass).
// The original ends with a sync instruction (no C form).
short func_00100610(short psm, short w, short h);

int func_001006D8(long long *env, short psm, short w, short h, short ztest, short zpsm) {
    env[1] = 0x4C;
    env[0] = ((((w + 0x3F) >> 6) & 0x3F) << 16) | ((psm & 0xF) << 24);
    env[3] = 0x4E;
    if (ztest == 0) {
        env[2] = (long long)(short)func_00100610(psm, w, h) | ((zpsm & 0xF) << 24) | (1LL << 32);
    } else {
        env[2] = (long long)(short)func_00100610(psm, w, h) | ((zpsm & 0xF) << 24);
    }
    env[5] = 0x18;
    env[4] = (long long)((0x800 - (short)(w >> 1)) << 4) | ((long long)(0x800 - (short)(h >> 1)) << 36);
    env[7] = 0x40;
    env[6] = ((long long)(w - 1) << 16) | ((long long)(h - 1) << 48);
    env[9] = 0x1A;
    env[8] |= 1;
    env[11] = 0x46;
    env[10] |= 1;
    env[13] = 0x45;
    if (psm & 2) {
        env[12] |= 1;
    } else {
        env[12] &= ~1;
    }
    env[15] = 0x47;
    if (ztest != 0) {
        env[14] = ((ztest & 3) << 17) | 0x10000;
    } else {
        env[14] = 0x30000;
    }
    return 8;
}
