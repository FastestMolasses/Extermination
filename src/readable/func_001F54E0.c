// NEARMISS func_001F54E0  (vram 0x001F54E0, 0x15C bytes) — readable companion C, NOT byte-identical.
//
// objdiff 98.51% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// idiom-13: mwcc 2.3.3 fills two bc1t delay slots with the next clamp's -127.0f upper-half load,
// which the original leaves as nop (ternary, goto-join, > and else-arm spellings and mwcc 991202
// measured, no change).
//
// The function links from the asm body in src/func_001F54E0.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Effect-kind colour jitter. One func_00122BB8() random value r gives a
// brightness k = col[3] * (-127 + 254 * r / 2^31) + 127; each of the three
// colour components becomes col[i] * k - 127, clamped to [-127, 127], and is
// stored to fx+0x80/+0x84/+0x88 with fx+0x8C = 1.0f. Then the effect's
// callback at fx+0x4C is called with fx (a1 still holds the 2^-31 constant's
// bits, 0x30000000, which is not an argument).
extern int func_00122BB8(void);

void func_001F54E0(char *fx, float *col) {
    float k;
    float r;
    float g;
    float b;

    k = col[3] * (-127.0f + 254.0f * (4.656613e-10f * (float)func_00122BB8()));
    k += 127.0f;
    r = col[0] * k - 127.0f;
    g = col[1] * k - 127.0f;
    b = col[2] * k - 127.0f;
    if (r < -127.0f) {
        r = -127.0f;
    }
    if (!(r <= 127.0f)) {
        r = 127.0f;
    }
    *(float *)(fx + 0x80) = r;
    if (g < -127.0f) {
        g = -127.0f;
    }
    if (!(g <= 127.0f)) {
        g = 127.0f;
    }
    *(float *)(fx + 0x84) = g;
    if (b < -127.0f) {
        b = -127.0f;
    }
    if (!(b <= 127.0f)) {
        b = 127.0f;
    }
    *(float *)(fx + 0x88) = b;
    *(int *)(fx + 0x8C) = 0x3F800000;
    (*(void (**)(char *))(fx + 0x4C))(fx);
}
