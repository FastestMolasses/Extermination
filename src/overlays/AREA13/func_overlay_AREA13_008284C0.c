// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// AREA13 overlay, runtime 0x00828500 (splat/link name 008284C0; overlay code
//  is linked 0x40 below where it runs), 0x758 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA13; lane A13C).
// Role: a thrown piece (no static reach). State 0: model 0x15 / 0x16 / 0x17 /
//  0x15 for +0xD 7..10 and a target: D_00810360 (+0xD 7, scale 0.5; +0xD 8),
//  a random point at radius 50 around D_00810360 (9) or (735, 225, 1250)
//  (10); start = its position, random yaw / roll, 1200-frame life, +0x30 =
//  &D_002759C8, +0x34 = 0x8284E0; falls into state 1. State 1: state 3 when
//  D_008106B8 == 2 and the short at 0x28A9A0 == 2, for +0xD 7 / 8 once
//  D_00810833 == 0xFF, or when the life runs out; spins +0xC0 by 3 degrees a
//  frame; moves along start -> target by t (+0x2C) on an arc of height 100 *
//  func_0011E2A8(pi * t); after t > 0.5 a func_0019A570 hit lands it (state
//  2); draws a model-0x3F5 marker and a func_001F9140 ground mark (colour
//  0x82D3B0); t += 1/120 and state 2 at t >= 1.5. State 2:
//  func_001EFD20(0x8000002F) (+0xD 7, 10) or (0x80000013) (8, 9) and
//  func_001F02C0(.., 0x449, 100), then state 3.
typedef struct { float x, y, z, w; } __attribute__((aligned(16))) Vec4;
extern Vec4 D_overlay_AREA13_0082D3B0;
extern char D_overlay_AREA13_008284E0[];
extern int D_002759C8;
extern unsigned char D_008106B8[];
extern unsigned char D_00810833[];
extern float D_70003A20[];
extern float D_00810360[];
extern float D_700031B0[];
extern float D_700036A0[];
extern float D_700036D0[];
extern float D_700038A0[];
extern float D_700038B0[];
extern int func_00122BB8(void);
extern float func_0011E2A8(float a);
extern float func_0011DE90(float a);
extern void func_00102948(void *dst, void *src);
extern void func_001028B8(void *dst, void *a, void *b);
extern void func_001028D0(void *dst, void *a, void *b);
extern void func_00102900(void *dst, void *src, float k);
extern void func_001029C0(void *m);
extern void func_00102C58(void *dst, void *a, void *b);
extern void func_00102918(void *a, void *b, void *c);
extern int func_0019A570(void *a, void *b, int c, int d);
extern int func_001CA7B0(void *m, float r);
extern void func_001C7900(void *m, void *c, int id, int a3);
extern int func_001C6120(int a, int b);
extern void func_001CA940(int h, int v);
extern void func_001F9140(void *a, void *b, void *c, Vec4 *col, float s);
extern void func_001B17A0(unsigned char *self);
extern void func_001EFD20(unsigned int msg, void *pos);
extern void func_001F02C0(void *pos, int id, float vol);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA13_008284C0(unsigned char *self) {
    Vec4 col = D_overlay_AREA13_0082D3B0;
    unsigned char *blk = self + 0x1F0;
    int h;
    unsigned char *hit;
    switch (self[4]) {
    case 0:
        switch (self[0xD]) {
        case 7:
            *(int *)(blk + 0x20) = 0x15;
            break;
        case 8:
            *(int *)(blk + 0x20) = 0x16;
            break;
        case 9:
            *(int *)(blk + 0x20) = 0x17;
            break;
        case 10:
            *(int *)(blk + 0x20) = 0x15;
            break;
        }
        switch (self[0xD]) {
        case 7:
            *(float *)(self + 0x60) = 0.5f;
            *(float *)(self + 0x64) = 0.5f;
            *(float *)(self + 0x68) = 0.5f;
            func_00102948(blk + 0x10, D_00810360);
            break;
        case 8:
            func_00102948(blk + 0x10, D_00810360);
            break;
        case 9:
            D_70003A20[0] = 6.2831855f * ((float)func_00122BB8() / 2147483648.0f);
            D_700038A0[0] = 50.0f * func_0011E2A8(D_70003A20[0]);
            D_700038A0[2] = 50.0f * func_0011DE90(D_70003A20[0]);
            *(float *)0x700038A4 = 0.0f;
            func_001028B8(blk + 0x10, D_700038A0, D_00810360);
            break;
        case 10:
            *(float *)(blk + 0x10) = 735.0f;
            *(float *)(blk + 0x14) = 225.0f;
            *(float *)(blk + 0x18) = 1250.0f;
            *(float *)(blk + 0x1C) = 1.0f;
            break;
        }
        func_00102948(blk, self + 0xB0);
        *(float *)(self + 0xC0) = 0.0f;
        *(float *)(self + 0xC4) = 6.2831855f * ((float)func_00122BB8() / 2147483648.0f) - 3.1415927f;
        *(float *)(self + 0xC8) = 6.2831855f * ((float)func_00122BB8() / 2147483648.0f) - 3.1415927f;
        *(int *)(blk + 0x24) = 0x4B0;
        *(float *)(blk + 0x28) = 0.0f;
        *(float *)(blk + 0x2C) = 0.0f;
        self[4] = 1;
        *(int **)(self + 0x30) = &D_002759C8;
        *(void **)(self + 0x34) = D_overlay_AREA13_008284E0;
        self[0] = 1;
    case 1:
        if (D_008106B8[0] == 2 && *(short *)0x28A9A0 == 2) {
            self[4] = 3;
            break;
        }
        if (D_00810833[0] == 0xFF) {
            switch (self[0xD]) {
            case 7:
            case 8:
                self[4] = 3;
                break;
            }
        }
        *(int *)(blk + 0x24) -= 1;
        if (*(int *)(blk + 0x24) < 0) {
            self[4] = 3;
        }
        *(float *)(self + 0xC0) = 3.1415927f * *(float *)(blk + 0x28) / 180.0f;
        *(float *)(blk + 0x28) += 3.0f;
        if (!(*(float *)(blk + 0x28) <= 180.0f)) {
            *(float *)(blk + 0x28) -= 360.0f;
        }
        func_001028D0(D_700038A0, blk + 0x10, blk);
        func_00102900(D_700038A0, D_700038A0, *(float *)(blk + 0x2C));
        func_001028B8(D_700038A0, D_700038A0, blk);
        *(float *)0x700038A4 += 100.0f * func_0011E2A8(3.1415927f * *(float *)(blk + 0x2C));
        *(float *)0x700038AC = 1.0f;
        if (!(*(float *)(blk + 0x2C) <= 0.5f) && func_0019A570(self + 0xB0, D_700038A0, 6, 0) != 0) {
            func_00102948(self + 0xB0, D_700031B0);
            self[4] = 2;
            break;
        }
        func_00102948(self + 0xB0, D_700038A0);
        func_001029C0(D_700036A0);
        func_00102C58(D_700036A0, D_700036A0, self + 0xC0);
        func_00102918(D_700036A0, D_700036A0, D_700038A0);
        *(float *)0x700038B0 = 1.0f;
        *(float *)0x700038B4 = 1.0f;
        *(float *)0x700038B8 = 1.0f;
        *(float *)0x700038BC = 1.0f;
        h = func_001CA7B0(D_700036D0, 10.0f);
        if (h >= 0) {
            func_001C7900(D_700036A0, D_700038B0, 0x3F5, 0);
            func_001CA940(h, func_001C6120(*(int *)0x28A59C, *(int *)(blk + 0x20)));
        }
        *(float *)0x700038A0 = *(float *)(self + 0xB0);
        *(float *)0x700038A4 = -10000.0f;
        *(float *)0x700038A8 = *(float *)(self + 0xB8);
        *(float *)0x700038AC = 1.0f;
        if (func_0019A570(self + 0xB0, D_700038A0, 6, 0) != 0) {
            hit = *(unsigned char **)0x700031D0;
            *(float *)0x700038A0 = *(float *)(hit + 0x24);
            *(float *)0x700038A4 = *(float *)(hit + 0x28);
            *(float *)0x700038A8 = *(float *)(hit + 0x2C);
            func_001F9140(D_00810360, D_700031B0, D_700038A0, &col, 7.0f);
        }
        *(float *)(blk + 0x2C) += 0.008333334f;
        if (!(*(float *)(blk + 0x2C) < 1.5f)) {
            func_00102948(self + 0xB0, D_700038A0);
            self[4] = 2;
        }
        func_001B17A0(self);
        break;
    case 2:
        switch (self[0xD]) {
        case 7:
            func_001EFD20(0x8000002F, self + 0xB0);
            func_001F02C0(self + 0xB0, 0x449, 100.0f);
            break;
        case 8:
        case 9:
            func_001EFD20(0x80000013, self + 0xB0);
            func_001F02C0(self + 0xB0, 0x449, 100.0f);
            break;
        case 10:
            func_001EFD20(0x8000002F, self + 0xB0);
            func_001F02C0(self + 0xB0, 0x449, 100.0f);
            break;
        }
        self[4] = 3;
        break;
    case 3:
        func_001AFC10(self);
        break;
    }
}
