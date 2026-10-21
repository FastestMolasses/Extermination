// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x700031C0 0x700031C8 0x700031F0
//
// Unless the byte at arg0+0x1F0 is 0x3C: calls
// func_0011E748(func_00102738(&D_00281B50, &D_00281B50)), sets
// a40 = func_00102760(D_00281B50), a50 = func_00102948(arg0 + 0xB0) and
// a60 = func_001028B8(a50, D_00281B50), and builds v70 with x / z = a60 +
// 4.5 * a40 and y = the float at arg2+4. When func_0019AFE0(arg0, arg2, v70,
// 6) is non-zero the floats at 0x700031C0 / 0x700031C8 are added to a60.x /
// a60.z; a60.x / a60.z are written to arg0+0xB0 / 0xB8 in both cases. Sets the
// word at 0x700031F0 to 1 when arg0+4 == 1 and arg0+5 == 0x1D. arg1 is not read.
// Byte-matched from C with mwcc 2.3.3 (decomp lane round 6, 2026-10-20); it was
// a 97.24 NEARMISS whose header called the residual an FP-load scheduling wall.
// It was MATCHING_GUIDE idiom-32: the three scratchpad globals are relocated
// externs (opted in with // SPAD:).

extern float D_700031C0;
extern float D_700031C8;
extern int D_700031F0;
extern float D_00281B50;

extern float func_00102738(float *a, float *b);
extern void func_0011E748(float x);
extern void func_00102760(float *out, float *src);
extern void func_00102948(float *out, char *src);
extern void func_001028B8(float *out, float *a, float *b);
extern int func_0019AFE0(char *a, char *b, float *v, int n);

void func_00176180(char *arg0, int arg1, char *arg2) {
    float a40[4];
    float a50[4];
    float a60[4];
    float v70[4];

    if (*(unsigned char *)(arg0 + 0x1F0) != 0x3C) {
        func_0011E748(func_00102738(&D_00281B50, &D_00281B50));
        func_00102760(a40, &D_00281B50);
        func_00102948(a50, arg0 + 0xB0);
        func_001028B8(a60, a50, &D_00281B50);
        v70[0] = a60[0] + 4.5f * a40[0];
        v70[2] = a60[2] + 4.5f * a40[2];
        v70[1] = *(float *)(arg2 + 4);
        if (func_0019AFE0(arg0, arg2, v70, 6) != 0) {
            a60[0] = a60[0] + D_700031C0;
            a60[2] = a60[2] + D_700031C8;
            *(float *)(arg0 + 0xB0) = a60[0];
            *(float *)(arg0 + 0xB8) = a60[2];
        } else {
            *(float *)(arg0 + 0xB0) = a60[0];
            *(float *)(arg0 + 0xB8) = a60[2];
        }
        if (*(unsigned char *)(arg0 + 4) == 1 && *(unsigned char *)(arg0 + 5) == 0x1D) {
            D_700031F0 = 1;
        }
    }
}
