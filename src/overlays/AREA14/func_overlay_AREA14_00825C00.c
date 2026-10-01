// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA14 overlay, runtime 0x00825C40 (splat/link name 00825C00; overlay code
//  is linked 0x40 below where it runs), 0x38C bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA14; lane A03C).
// Role: script callback (script 0x828120, op09 record 0x828320), called with
//  (self, record, work). Record +4 0: +0x2E = 1, work +0x10 = 80, +4 1. +4 1:
//  work +0x10 counts down by 1; at 0: func_00102948(D_008105E0, D_00810360),
//  D_008105E4 += 6, func_00102948(record + 0x10, D_008105D0), +4 2, +5 0,
//  self +0x38 = 0, func_001FB9F0(0x451, 0x1000, 0x1000, 0x1000). +4 2: +5 0
//  calls func_001AEDE0(4, 0) and func_001B0C60(0x11, 0, 0) once D_00810364 >
//  810 (+5 1); +5 1 returns 1 once D_00810364 > 900. Otherwise +0x38 grows by
//  0.01 to at most 2.0 and is added to +0xB4; func_00183010(D_008102B0, (0,
//  +0x38, 0, 1)); while D_008105E4 < 816 the D_008105E0 copy and + 6 repeat;
//  while work +0x10 < 180 it counts up and D_008105D0 (from
//  func_001028D0(D_008105D0, work + 0x20, record + 0x10)) becomes record
//  +0x10 + D_008105D0 * (1 + func_0011E2A8(pi * n / 180 - pi / 2)) / 2.
//  Returns 0.
extern float D_00810360[4];
extern float D_008105D0[4];
extern float D_008105E0[4];
extern char D_008102B0[];
extern float D_700038A0[4];
extern void func_00102948(void *dst, void *src);
extern void func_001FB9F0(int id, int a1, int a2, int a3);
extern void func_001AEDE0(int a0, int a1);
extern void func_001B0C60(int a0, int a1, int a2);
extern void func_00183010(void *a0, void *a1);
extern float func_0011E2A8(float a);
extern void func_001028D0(void *dst, void *a, void *b);

int func_overlay_AREA14_00825C00(unsigned char *self, unsigned char *rec, unsigned char *w) {
    float t;
    switch (rec[4]) {
    case 0:
        *(short *)(self + 0x2E) = 1;
        *(float *)(w + 0x10) = 80.0f;
        rec[4]++;
        break;
    case 1:
        *(float *)(w + 0x10) -= 1.0f;
        if (!*(float *)(w + 0x10)) {
            func_00102948(D_008105E0, D_00810360);
            D_008105E0[1] += 6.0f;
            func_00102948(rec + 0x10, D_008105D0);
            *(float *)(w + 0x10) = 0.0f;
            rec[4]++;
            rec[5] = 0;
            *(float *)(self + 0x38) = 0.0f;
            func_001FB9F0(0x451, 0x1000, 0x1000, 0x1000);
        }
        break;
    case 2:
        switch (rec[5]) {
        case 0:
            if (!(D_00810360[1] <= 810.0f)) {
                func_001AEDE0(4, 0);
                func_001B0C60(0x11, 0, 0);
                rec[5]++;
            }
            break;
        case 1:
            if (!(D_00810360[1] <= 900.0f)) {
                return 1;
            }
            break;
        }
        *(float *)(self + 0x38) += 0.01f;
        if (!(*(float *)(self + 0x38) <= 2.0f)) {
            *(float *)(self + 0x38) = 2.0f;
        }
        *(float *)(self + 0xB4) += *(float *)(self + 0x38);
        D_700038A0[0] = 0.0f;
        D_700038A0[1] = *(float *)(self + 0x38);
        D_700038A0[2] = 0.0f;
        D_700038A0[3] = 1.0f;
        func_00183010(D_008102B0, D_700038A0);
        if (D_008105E0[1] < 816.0f) {
            func_00102948(D_008105E0, D_00810360);
            D_008105E0[1] += 6.0f;
        }
        if (*(float *)(w + 0x10) < 180.0f) {
            *(float *)(w + 0x10) += 1.0f;
            t = (1.0f + func_0011E2A8(3.1415927f * (*(float *)(w + 0x10) / 180.0f) - 1.5707964f)) / 2.0f;
            func_001028D0(D_008105D0, w + 0x20, rec + 0x10);
            D_008105D0[0] = *(float *)(rec + 0x10) + D_008105D0[0] * t;
            D_008105D0[1] = *(float *)(rec + 0x14) + D_008105D0[1] * t;
            D_008105D0[2] = *(float *)(rec + 0x18) + D_008105D0[2] * t;
        }
        break;
    }
    return 0;
}
