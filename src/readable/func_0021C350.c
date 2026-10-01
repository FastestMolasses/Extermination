// NEARMISS func_0021C350  (vram 0x0021C350, 0x94 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 99.73% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// One FPU register pair: the original loads the pending damage into f2 and the zero into f0 for
// the first compare; mwcc 2.3.3 / 2.4 swap them (cached, re-read, early-return and operand-order
// spellings measured).
//
// The function links from the asm body in src/func_0021C350.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

void func_0021C350(char *e) {
    float d = *(float *)(e + 0x224);

    if (d != 0.0f) {
        *(float *)(e + 0x220) = *(float *)(e + 0x220) - d;
        *(float *)(e + 0x224) = 0.0f;
        if (*(float *)(e + 0x220) <= 35.0f) {
            *(unsigned char *)(e + 0x235) &= 0xFE;
            *(unsigned char *)(e + 0x235) |= 1;
        }
        if (*(float *)(e + 0x220) <= 0.0f) {
            *(float *)(e + 0x220) = 0.0f;
            *(char *)e = 2;
        }
    }
}
