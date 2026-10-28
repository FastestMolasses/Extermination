// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a CodeWarrior asm body;
// readable companion src/readable/func_001F54E0.c was 98.51%). Fix read from the
// original instructions:
// - Each clamp is a pair of conditional expressions (`v = v < -127 ? -127 : v;
//   v = v > 127 ? 127 : v;`), the spelling of the byte-matched colour clamps in
//   func_001C50B0. The if-statement form let mwcc fill the upper-clamp branch delay
//   slots with the next clamp's constant load, which the original leaves empty.
//
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
    r = r < -127.0f ? -127.0f : r;
    r = r > 127.0f ? 127.0f : r;
    *(float *)(fx + 0x80) = r;
    g = g < -127.0f ? -127.0f : g;
    g = g > 127.0f ? 127.0f : g;
    *(float *)(fx + 0x84) = g;
    b = b < -127.0f ? -127.0f : b;
    b = b > 127.0f ? 127.0f : b;
    *(float *)(fx + 0x88) = b;
    *(int *)(fx + 0x8C) = 0x3F800000;
    (*(void (**)(char *))(fx + 0x4C))(fx);
}
