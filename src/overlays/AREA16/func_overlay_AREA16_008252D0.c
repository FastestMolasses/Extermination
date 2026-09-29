// NEARMISS func_overlay_AREA16_008252D0 (92.05%, mwcc 2.3.3; linked from its splat .s)
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA16 overlay, runtime 0x00825310 (splat/link name 008252D0; overlay code is
// linked 0x40 below where it runs), 0x314 bytes.
// Role (read from the instructions): state 0 (after func_001B0FD0,
//  func_001C6380): +0x2E8 = +0xB4, +0x2EC = 0, +0x208 = rand seed, +0x200 /
//  +0x204 = +0xB0 / +0xB8, four +0x1F0 rand fractions. State 1: unless
//  D_00810809 == 2 with D_70003B92 set, +0x2EC phase += 0.01 (wrap at pi),
//  +0xB4 = +0x2E8 + 10 sin, func_001C6380, +0x4C when func_001B17A0; then four
//  packets at (+0x200, 115, +0x204) sized i/4 (0x82A140 / 0x82A148 = 10 *
//  size) with seeded randoms (func_001CFAE0, func_001CFBE0 table 0x82A130),
//  each +0x1F0 value += 0.0025 size, wrapping past 2. State 3/other
//  func_001AFC10.
// Divergence: list scheduling in the case-1 packet loop: the original
//  materialises the 10.0 constant first, converts i+1 in f1 and stores
//  0x82A140 before 0x82A148; mwcc 2.3.3 orders the float constants and the two
//  stores differently (all 720 declaration orders, statement orders and a
//  chained store tried). The multiset audit is clean.
extern unsigned char D_00810809;
extern unsigned char D_70003B92;
extern float D_70003A20;
extern char D_700036A0[];
extern float D_700036D0[];
extern float D_overlay_AREA16_0082A140;
extern float D_overlay_AREA16_0082A148;
extern char D_overlay_AREA16_0082A130[];
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern int func_00122BB8(void);
extern float func_0011E2A8(float x);
extern int func_001B17A0(unsigned char *self);
extern void func_001029C0(void *m);
extern void func_001CFAE0(void *dst, int a1, void *src, float f12, float f13, float f14, float f15);
extern void func_001CFBE0(int a0, int a1, void *a2, void *a3, int t0);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA16_008252D0(unsigned char *self) {
    float r;
    int seed;
    int i;
    float *p;
    int pkt[24];
    float *blk = (float *)(self + 0x1F0);
    switch (self[4]) {
    case 0:
        if (func_001B0FD0(self) == 0) {
            func_001C6380(self);
            *(float *)(self + 0x2E8) = *(float *)(self + 0xB4);
            *(float *)(self + 0x2EC) = 0.0f;
            ((int *)blk)[6] = func_00122BB8();
            blk[4] = *(float *)(self + 0xB0);
            blk[5] = *(float *)(self + 0xB8);
            for (i = 0; i < 4; i++) {
                *blk = (float)func_00122BB8() / 2147483648.0f;
                blk++;
            }
        }
        break;
    case 1:
        if (D_00810809 != 2 || D_70003B92 == 0) {
            p = (float *)(self + 0x1F0) + 0x3F;
            *(float *)(self + 0x2EC) += 0.01f;
            if (*(float *)(self + 0x2EC) > 3.1415927f) {
                *p = -3.1415927f;
            }
            *(float *)(self + 0xB4) = *(float *)(self + 0x2E8) + 10.0f * func_0011E2A8(*p);
            func_001C6380(self);
            if (func_001B17A0(self) != 0) {
                (*(void (**)(unsigned char *))(self + 0x4C))(self);
            }
        }
        seed = ((int *)blk)[6];
        func_001029C0(D_700036A0);
        D_700036D0[0] = blk[4];
        D_700036D0[2] = blk[5];
        D_700036D0[1] = 115.0f;
        for (i = 0; i < 4; i++) {
            D_70003A20 = (float)(i + 1) / 4.0f;
            r = (float)((seed >> 16) & 0xFFFF);
            r = r / 65535.0f;
            seed = seed * 37 + 11;
            D_overlay_AREA16_0082A140 = D_overlay_AREA16_0082A148 = 10.0f * D_70003A20;
            r += 0.0001f;
            func_001CFAE0(pkt, 0, D_700036A0, *blk, r, 1.0f, 0.3f);
            func_001CFBE0(0, 1, D_overlay_AREA16_0082A130, pkt, 1);
            *blk += 0.0025f * D_70003A20;
            if (*blk > 2.0f) {
                *blk -= 1.0f;
            }
            blk++;
        }
        break;
    case 3:
    default:
        func_001AFC10(self);
        break;
    }
}
