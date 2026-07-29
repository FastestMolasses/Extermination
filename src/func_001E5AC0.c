// NEARMISS func_001E5AC0  (vram 0x001E5AC0, 0xD00 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 92.45% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// 92.45% on mwcc 2.3.3 (991202 89.19%). Logic fully recovered (112 differing rows of 832, all register-name / local-scheduling); GPR saved-register coloring was CRACKED this session (declaring `p = e;` before `row`/`i` fixed the s1/s3/s4 3-cycle permutation, and the `+=` compound-assign idiom was a...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

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
extern int D_700038A0;
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
    float s13, st;
    float y;
    float uv;
    int n;
    int sp90[24];

    fx = D_008105D0;
    seed = arg2;
    e = (char *)arg0 + 0x1F0;
    n = float_to_int(fx);
    fy = D_008105D4;
    *(float *)0x700038B0 = (float)((n + 0x186A0) % 160);
    n = float_to_int(fy);
    fz = D_008105D8;
    *(float *)0x700038B4 = (float)((n + 0x186A0) % 160);
    n = float_to_int(fz);
    *(float *)0x700038B8 = (float)((n + 0x186A0) % 160);
    *(float *)0x700038B0 += (fx - (float)float_to_int(fx));
    *(float *)0x700038B4 += (fy - (float)float_to_int(fy));
    mode = arg1 & 2;
    p = e;
    row = D_00255050;
    i = 0;
    *(float *)0x700038B8 += (fz - (float)float_to_int(fz));
    s13 = 1.3f * fparg0;
    st = 0.004f * fparg0;
    do {
        if (mode != 0) {
            *(float *)0x70003A20 = *(float *)row;
            *(float *)0x70003A20 += 0.5f * (*(float *)row * func_0011E2A8(6.2831855f * *(float *)(p + 0x18)));
        } else {
            *(float *)0x70003A20 = *(float *)row;
            *(float *)0x70003A20 += 0.5f * (*(float *)row * func_0011E2A8(6.2831855f * *(float *)(p + 0x18)));
        }
        *(float *)0x700038A0 = -160.0f * ((float)i / 3.0f);
        *(float *)0x700038A4 = 0.0f;
        *(float *)0x700038A8 = -130.0f;
        *(float *)0x700038AC = 1.0f;
        while (*(float *)0x700038A0 < 0.0f) {
            *(volatile float *)0x700038A0 = *(volatile float *)0x700038A0 + 140.0f;
        }
        *(float *)0x700038A0 -= 70.0f;
        func_001029C0(&D_700036A0);
        func_00102B08(&D_700036A0, &D_700036A0, (3.1415927f * *(float *)0x70003A20) / 180.0f);
        func_001026A0(&D_700036D0, &D_700036A0, &D_700038A0);
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
        uv = (float)((seed >> 0x10) & 0xFFFF) / 65535.0f;
        seed = (seed * 0x25) + 0xB;
        func_001CFAE0(sp90, 0, &D_700036A0, *(float *)p, uv + 0.0001f, 1.0f, 0.000001f);
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
    fx = D_008105D0;
    n = float_to_int(fx);
    fy = D_008105D4;
    *(float *)0x700038B0 = (float)((n + 0x186A0) % 80);
    n = float_to_int(fy);
    fz = D_008105D8;
    *(float *)0x700038B4 = (float)((n + 0x186A0) % 80);
    n = float_to_int(fz);
    *(float *)0x700038B8 = (float)((n + 0x186A0) % 80);
    *(float *)0x700038B0 += (fx - (float)float_to_int(fx));
    *(float *)0x700038B4 += (fy - (float)float_to_int(fy));
    p = e + 0xC;
    row = D_002550E0;
    i = 3;
    *(float *)0x700038B8 += (fz - (float)float_to_int(fz));
    *(float *)0x700038B0 /= 2.0f;
    *(float *)0x700038B4 /= 2.0f;
    *(float *)0x700038B8 /= 2.0f;
    do {
        if (mode != 0) {
            *(float *)0x70003A20 = *(float *)row;
            *(float *)0x70003A20 += 0.5f * (*(float *)row * func_0011E2A8(6.2831855f * *(float *)(p + 0x18)));
        } else {
            *(float *)0x70003A20 = *(float *)row;
            *(float *)0x70003A20 += 0.5f * (*(float *)row * func_0011E2A8(6.2831855f * *(float *)(p + 0x18)));
        }
        *(float *)0x700038A0 = -40.0f * ((float)(i - 3) / 3.0f);
        *(float *)0x700038A4 = 0.0f;
        *(float *)0x700038A8 = -30.0f;
        *(float *)0x700038AC = 1.0f;
        while (*(float *)0x700038A0 < 0.0f) {
            *(volatile float *)0x700038A0 = *(volatile float *)0x700038A0 + 40.0f;
        }
        *(float *)0x700038A0 -= 20.0f;
        func_001029C0(&D_700036A0);
        func_00102B08(&D_700036A0, &D_700036A0, (3.1415927f * *(float *)0x70003A20) / 180.0f);
        func_001026A0(&D_700036D0, &D_700036A0, &D_700038A0);
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
        func_00102900(&D_00255000, row + 0x10, 1.0f);
        func_00102900(&D_00254FE0, row + 0x20, s13);
        func_00102948(&D_00255010, &D_00255000);
        func_00102948(&D_00254FF0, &D_00254FE0);
        uv = (float)((seed >> 0x10) & 0xFFFF) / 65535.0f;
        seed = (seed * 0x25) + 0xB;
        func_001CFAE0(sp90, 0, &D_700036A0, *(float *)p, uv + 0.0001f, 1.0f, 0.000001f);
        func_001CFFE0(3, 3, &D_00254FC0, sp90, 1);
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
    } while (i < 6);
    func_0021B9A0(1, 0.0f, 0.0f);
    y = *(float *)(e + 0x30) - 75.0f;
    *(float *)0x70003A20 = y;
    if (!(y <= 0.0f)) {
        *(float *)0x70003A20 /= 52.0f;
        y = 56.0f * *(float *)0x70003A20;
        *(float *)0x700038A0 = y;
        *(float *)0x700038A4 = y;
        *(float *)0x700038A8 = y;
        *(float *)0x700038AC = 0.0f;
        func_00103230(&D_700038A0, &D_700038A0, fparg0);
        col = func_00128250(*(float *)0x700038A0);
        col = col | (func_00128250(*(float *)0x700038A4) << 8);
        col = col | (func_00128250(*(float *)0x700038A8) << 16);
        col = col | (func_00128250(*(float *)0x700038AC) << 24);
        func_001E6F60(3, 0x7000, 0x7900, 0x9000, 0x8700, 0xFFFFFF, col);
    }
}
