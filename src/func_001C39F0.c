// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// fparg0-gated motion driver. Runs only when fparg0 != 0. Reads a mode from
// (arg1[0xE4] >> 8): mode 0 or 4 -> simple path (publish 1.0/0/fparg0/0.0 into
// the global block at 0x70003600..0x7000360C, call func_001026A0 to advance, then
// add the 0x70003610/14/18 deltas into arg0[0xB0/B4/B8]); otherwise -> the full
// path that also seeds 0x70003600 with (fparg0/2)*func_0011E2A8(arg1[0xDC]),
// recomputes arg1[0xDC] via func_001B1470(2*pi * ((n - arg0[0x3C]) / n)) where
// n = (float)func_001C6160(arg0), then after advancing also subtracts
// arg1[0x80/84/88]/2 from arg0[0xB0/B4/B8].
//
// Matched 100.0 with mwcc 2.3.3; the pinned 991202 build reaches 91.98%. Keys:
// (1) D_70003600/D_70003610 are extern float symbols so the func_001026A0 call
// args emit %hi/%lo relocs (the 0x7000360x stores stay raw int-pointer casts);
// (2) a single shared `float two = 2.0f`; (3) the trailing subtract block only
// reproduces the target's FP register pairing when written as compound `-=`.
extern void func_001026A0(float *a, char *b, float *c);
extern float func_0011E2A8(float x);
extern float func_001B1470(float x);
extern int func_001C6160(char *p);
extern float D_70003600;
extern float D_70003610;

void func_001C39F0(char *arg0, char *arg1, float fparg0) {
    int mode;
    float n;
    float two;

    if (fparg0 != 0.0f) {
        mode = *(int *)(arg1 + 0xE4) >> 8;
        if (mode == 0 || mode == 4) {
            *(float *)0x7000360C = 1.0f;
            *(int *)0x70003604 = 0;
            *(float *)0x70003608 = fparg0;
            *(float *)0x70003600 = 0.0f;
            func_001026A0(&D_70003610, arg0 + 0xD0, &D_70003600);
            *(float *)(arg0 + 0xB0) = *(float *)(arg0 + 0xB0) + *(float *)0x70003610;
            *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0xB4) + *(float *)0x70003614;
            *(float *)(arg0 + 0xB8) = *(float *)(arg0 + 0xB8) + *(float *)0x70003618;
        } else {
            *(float *)0x7000360C = 1.0f;
            *(int *)0x70003604 = 0;
            *(float *)0x70003608 = fparg0;
            two = 2.0f;
            *(float *)0x70003600 = (fparg0 / two) * func_0011E2A8(*(float *)(arg1 + 0xDC));
            n = (float)func_001C6160(arg0);
            *(float *)(arg1 + 0xDC) = func_001B1470(6.2831855f * ((n - *(float *)(arg0 + 0x3C)) / n));
            func_001026A0(&D_70003610, arg0 + 0xD0, &D_70003600);
            *(float *)(arg0 + 0xB0) = *(float *)(arg0 + 0xB0) + *(float *)0x70003610;
            *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0xB4) + *(float *)0x70003614;
            *(float *)(arg0 + 0xB8) = *(float *)(arg0 + 0xB8) + *(float *)0x70003618;
            *(float *)(arg0 + 0xB0) -= *(float *)(arg1 + 0x80) / two;
            *(float *)(arg0 + 0xB4) -= *(float *)(arg1 + 0x84) / two;
            *(float *)(arg0 + 0xB8) -= *(float *)(arg1 + 0x88) / two;
        }
    }
}
