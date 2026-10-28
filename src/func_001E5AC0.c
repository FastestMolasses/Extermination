// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x700038A0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 92.45% NEARMISS).
// Fixes read from the original instructions:
//  - the second bank uses its own locals (camera copies gx/gy/gz declared
//    right after fx/fy/fz, and row2/i2/p2 declared last as p2, row2, i2);
//  - each bank's setup (mode, row, scale constants, counter, phase pointer)
//    follows the third wrap-fraction add, and bank 2's halving divides come
//    before its row/counter/pointer setup;
//  - the first camera read is the call argument `float_to_int(fx = ...)`;
//  - the UV jitter is built in steps: convert, `/= 65535`, `+= 0.0001`;
//  - the 0x700038A0 offset vector is the relocated scratchpad extern (SPAD),
//    and the angle wrap is a compound `+= 140` / `+= 40` on it;
//  - the closing func_0021B9A0(1, 0, 0) spells its shared zero as
//    `(w = 0.0f), w` (twin func_001F5940).
//
// Per-frame flame/plume effect built from two banks of 3 billboarded strips.
// arg0 = effect object (state array at +0x1F0: [i] = phase float, [i+0x18] =
// wave phase), arg1 bit1 = direction flag, arg2 = LCG seed for the per-strip UV
// jitter (seed = seed*0x25 + 0xB), fparg0 = frame delta.
// Each strip: build a rotation about the wobble angle, compose with the strip's
// local offset and the camera matrix (D_008105D0), scale the two tangent vectors
// (D_00255050 / D_002550E0 rows: +0x00 amplitude, +0x08 rate, +0x10/+0x20 axes),
// emit the strip via func_001CFAE0/func_001CFFE0, then advance the phase.
// Bank 1: 3 strips, 140-degree sweep, radius 130. Bank 2: 3 strips, 40-degree
// sweep, radius 30, half-scale offsets. Finally a fade-in overlay quad once the
// object's +0x220 timer passes 75.
extern int float_to_int(float f);
extern void func_001026A0(void *dst, void *src, void *xf);
extern void func_001028B8(void *dst, void *a, void *b);
extern void func_00102798(void *dst, void *src);
extern void func_00102900(void *dst, void *src, float s);
extern void func_00102948(void *dst, void *src);
extern void func_001029C0(void *m);
extern void func_00102B08(void *a, void *b, float ang);
extern void func_00103230(void *a, void *b, float f);
extern float func_0011E2A8(float a);
extern int func_00128250(float f);
extern void func_001CFAE0(void *out, int n, void *p, float a, float b, float c, float d);
extern void func_001CFFE0(int a, int b, void *src, void *out, int e);
extern void func_0021B9A0(int id, float a, float b);
extern void func_001E6F60(int a, int b, int c, int d, int e, int f, int g);

extern int D_00254F30;
extern int D_00254F50;
extern int D_00254F60;
extern int D_00254F70;
extern int D_00254F80;
extern int D_00254FC0;
extern int D_00254FE0;
extern int D_00254FF0;
extern int D_00255000;
extern int D_00255010;
extern char D_00255050[];
extern char D_002550E0[];
extern float D_008105D0;
extern float D_008105D4;
extern float D_008105D8;
extern int D_00810610;
extern int D_700036A0;
extern int D_700036D0;
extern int D_700036E0;
extern float D_700038A0[4];
extern int D_700038B0;

void func_001E5AC0(void *arg0, int arg1, int arg2, float fparg0) {
    int seed;
    int i;
    char *e;
    char *row;
    char *p;
    int mode;
    int col;
    float fx, fy, fz;
    float gx, gy, gz;
    float s13, st;
    float y;
    float uv;
    int n;
    char *p2;
    char *row2;
    int i2;
    int sp90[24];

    seed = arg2;
    e = (char *)arg0 + 0x1F0;
    n = float_to_int(fx = D_008105D0);
    fy = D_008105D4;
    *(float *)0x700038B0 = (float)((n + 0x186A0) % 160);
    n = float_to_int(fy);
    fz = D_008105D8;
    *(float *)0x700038B4 = (float)((n + 0x186A0) % 160);
    n = float_to_int(fz);
    *(float *)0x700038B8 = (float)((n + 0x186A0) % 160);
    *(float *)0x700038B0 += (fx - (float)float_to_int(fx));
    *(float *)0x700038B4 += (fy - (float)float_to_int(fy));
    *(float *)0x700038B8 += (fz - (float)float_to_int(fz));
    mode = arg1 & 2;
    row = D_00255050;
    s13 = 1.3f * fparg0;
    st = 0.004f * fparg0;
    i = 0;
    p = e;
    do {
        if (mode != 0) {
            *(float *)0x70003A20 = *(float *)row;
            *(float *)0x70003A20 += 0.5f * (*(float *)row * func_0011E2A8(6.2831855f * *(float *)(p + 0x18)));
        } else {
            *(float *)0x70003A20 = *(float *)row;
            *(float *)0x70003A20 += 0.5f * (*(float *)row * func_0011E2A8(6.2831855f * *(float *)(p + 0x18)));
        }
        D_700038A0[0] = -160.0f * ((float)i / 3.0f);
        D_700038A0[1] = 0.0f;
        D_700038A0[2] = -130.0f;
        D_700038A0[3] = 1.0f;
        while (D_700038A0[0] < 0.0f) {
            D_700038A0[0] += 140.0f;
        }
        D_700038A0[0] -= 70.0f;
        func_001029C0(&D_700036A0);
        func_00102B08(&D_700036A0, &D_700036A0, (3.1415927f * *(float *)0x70003A20) / 180.0f);
        func_001026A0(&D_700036D0, &D_700036A0, D_700038A0);
        func_001028B8(&D_700036D0, &D_700036D0, &D_008105D0);
        *(float *)0x700038B0 = 0.0f;
        *(float *)0x700038B4 = 0.0f;
        *(float *)0x700038B8 = 30.0f;
        *(float *)0x700038BC = 1.0f;
        func_00102798(&D_700036E0, &D_00810610);
        *(int *)0x70003710 = 0;
        *(int *)0x70003714 = 0;
        *(int *)0x70003718 = 0;
        *(float *)0x7000371C = 1.0f;
        func_001026A0(&D_700038B0, &D_700036E0, &D_700038B0);
        func_001028B8(&D_700036D0, &D_700036D0, &D_700038B0);
        *(float *)0x700036DC = 1.0f;
        func_00102900(&D_00254F70, row + 0x10, 1.0f);
        func_00102900(&D_00254F50, row + 0x20, s13);
        func_00102948(&D_00254F80, &D_00254F70);
        func_00102948(&D_00254F60, &D_00254F50);
        uv = (float)((seed >> 0x10) & 0xFFFF);
        uv /= 65535.0f;
        uv += 0.0001f;
        seed = (seed * 0x25) + 0xB;
        func_001CFAE0(sp90, 0, &D_700036A0, *(float *)p, uv, 1.0f, 0.000001f);
        func_001CFFE0(3, 3, &D_00254F30, sp90, 1);
        if (mode != 0) {
            *(float *)(p + 0x18) -= st;
            y = *(float *)p - 1.5f * (*(float *)(row + 8) * fparg0);
            *(float *)p = y;
            if (y < 1.0f) {
                *(float *)p += 1.0f;
            }
        } else {
            *(float *)(p + 0x18) += st;
            y = *(float *)p + 1.5f * (*(float *)(row + 8) * fparg0);
            *(float *)p = y;
            if (!(y <= 2.0f)) {
                *(float *)p -= 1.0f;
            }
        }
        i += 1;
        p += 4;
        row += 0x30;
    } while (i < 3);
    func_0021B9A0(2, 0.0f, 0.0f);
    func_0021B9A0(3, 0.0f, 70.0f);
    n = float_to_int(gx = D_008105D0);
    gy = D_008105D4;
    *(float *)0x700038B0 = (float)((n + 0x186A0) % 80);
    n = float_to_int(gy);
    gz = D_008105D8;
    *(float *)0x700038B4 = (float)((n + 0x186A0) % 80);
    n = float_to_int(gz);
    *(float *)0x700038B8 = (float)((n + 0x186A0) % 80);
    *(float *)0x700038B0 += (gx - (float)float_to_int(gx));
    *(float *)0x700038B4 += (gy - (float)float_to_int(gy));
    *(float *)0x700038B8 += (gz - (float)float_to_int(gz));
    *(float *)0x700038B0 /= 2.0f;
    *(float *)0x700038B4 /= 2.0f;
    *(float *)0x700038B8 /= 2.0f;
    row2 = D_002550E0;
    i2 = 3;
    p2 = e + 0xC;
    do {
        if (mode != 0) {
            *(float *)0x70003A20 = *(float *)row2;
            *(float *)0x70003A20 += 0.5f * (*(float *)row2 * func_0011E2A8(6.2831855f * *(float *)(p2 + 0x18)));
        } else {
            *(float *)0x70003A20 = *(float *)row2;
            *(float *)0x70003A20 += 0.5f * (*(float *)row2 * func_0011E2A8(6.2831855f * *(float *)(p2 + 0x18)));
        }
        D_700038A0[0] = -40.0f * ((float)(i2 - 3) / 3.0f);
        D_700038A0[1] = 0.0f;
        D_700038A0[2] = -30.0f;
        D_700038A0[3] = 1.0f;
        while (D_700038A0[0] < 0.0f) {
            D_700038A0[0] += 40.0f;
        }
        D_700038A0[0] -= 20.0f;
        func_001029C0(&D_700036A0);
        func_00102B08(&D_700036A0, &D_700036A0, (3.1415927f * *(float *)0x70003A20) / 180.0f);
        func_001026A0(&D_700036D0, &D_700036A0, D_700038A0);
        func_001028B8(&D_700036D0, &D_700036D0, &D_008105D0);
        *(float *)0x700038B0 = 0.0f;
        *(float *)0x700038B4 = 0.0f;
        *(float *)0x700038B8 = 30.0f;
        *(float *)0x700038BC = 1.0f;
        func_00102798(&D_700036E0, &D_00810610);
        *(int *)0x70003710 = 0;
        *(int *)0x70003714 = 0;
        *(int *)0x70003718 = 0;
        *(float *)0x7000371C = 1.0f;
        func_001026A0(&D_700038B0, &D_700036E0, &D_700038B0);
        func_001028B8(&D_700036D0, &D_700036D0, &D_700038B0);
        *(float *)0x700036DC = 1.0f;
        func_00102900(&D_00255000, row2 + 0x10, 1.0f);
        func_00102900(&D_00254FE0, row2 + 0x20, s13);
        func_00102948(&D_00255010, &D_00255000);
        func_00102948(&D_00254FF0, &D_00254FE0);
        uv = (float)((seed >> 0x10) & 0xFFFF);
        uv /= 65535.0f;
        uv += 0.0001f;
        seed = (seed * 0x25) + 0xB;
        func_001CFAE0(sp90, 0, &D_700036A0, *(float *)p2, uv, 1.0f, 0.000001f);
        func_001CFFE0(3, 3, &D_00254FC0, sp90, 1);
        if (mode != 0) {
            *(float *)(p2 + 0x18) -= st;
            y = *(float *)p2 - 1.5f * (*(float *)(row2 + 8) * fparg0);
            *(float *)p2 = y;
            if (y < 1.0f) {
                *(float *)p2 += 1.0f;
            }
        } else {
            *(float *)(p2 + 0x18) += st;
            y = *(float *)p2 + 1.5f * (*(float *)(row2 + 8) * fparg0);
            *(float *)p2 = y;
            if (!(y <= 2.0f)) {
                *(float *)p2 -= 1.0f;
            }
        }
        i2 += 1;
        p2 += 4;
        row2 += 0x30;
    } while (i2 < 6);
    {
        float w = 0.0f;
        func_0021B9A0(1, (w = 0.0f), w);
    }
    y = *(float *)(e + 0x30) - 75.0f;
    *(float *)0x70003A20 = y;
    if (!(y <= 0.0f)) {
        *(float *)0x70003A20 /= 52.0f;
        y = 56.0f * *(float *)0x70003A20;
        D_700038A0[0] = y;
        D_700038A0[1] = y;
        D_700038A0[2] = y;
        D_700038A0[3] = 0.0f;
        func_00103230(D_700038A0, D_700038A0, fparg0);
        col = func_00128250(D_700038A0[0]);
        col = col | (func_00128250(D_700038A0[1]) << 8);
        col = col | (func_00128250(D_700038A0[2]) << 16);
        col = col | (func_00128250(D_700038A0[3]) << 24);
        func_001E6F60(3, 0x7000, 0x7900, 0x9000, 0x8700, 0xFFFFFF, col);
    }
}
