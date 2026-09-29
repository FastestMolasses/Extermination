// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA16 overlay, runtime 0x008239B0 (splat/link name 00823970; overlay code is
// linked 0x40 below where it runs), 0x370 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA16; lane A15C).
// Role: state 0: builds the self + 0xD0 matrix (func_001029C0, func_00102C58
//  by +0xC0, func_00102918 by +0xB0), +0x1F0 = 1, +0x1F4 = +0x1F8 = 0, +0x1FC
//  = rand, state 1, +5 = 0 (falls through). State 1: +5 0 fades +0x1F4 in by
//  1/30 to 1 then +5 1; 1 advances +0x1F8 by 0.01 and past 3.5 fades +0x1F4
//  out by 1/60, state 3 below 0. Draws two packets (func_001CCF70 at (304,
//  210, 456) and (304, 178, 456), func_001CFAE0 with a seeded random,
//  func_001CFBE0 tables 0x828EF0 / 0x828F80); +0x1F0 += 0.007, wraps past 2.
//  States 2/3 func_001AFC10.
extern char D_700036A0[];
extern float D_700036D0[];
extern char D_overlay_AREA16_00828EF0[];
extern char D_overlay_AREA16_00828F80[];
extern void func_001029C0(void *m);
extern void func_00102C58(void *dst, void *src, void *rot);
extern void func_00102918(void *dst, void *src, void *pos);
extern int func_00122BB8(void);
extern int func_001CCF70(void *p);
extern void func_001CFAE0(void *dst, int a1, void *src, float f12, float f13, float f14, float f15);
extern void func_001CFBE0(int a0, int a1, void *a2, void *a3, int t0);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA16_00823970(unsigned char *self) {
    int h;
    int pkt[24];
    int seed;
    float *blk = (float *)(self + 0x1F0);
    float r;
    switch (self[4]) {
    case 0:
        func_001029C0(self + 0xD0);
        func_00102C58(self + 0xD0, self + 0xD0, self + 0xC0);
        func_00102918(self + 0xD0, self + 0xD0, self + 0xB0);
        blk[0] = 1.0f;
        blk[1] = 0.0f;
        blk[2] = 0.0f;
        ((int *)blk)[3] = func_00122BB8();
        self[4] = 1;
        self[5] = 0;
    case 1:
        switch (self[5]) {
        case 0:
            blk[1] += 0.033333335f;
            if (blk[1] > 1.0f) {
                blk[1] = 1.0f;
                self[5]++;
            }
            break;
        case 1:
            blk[2] += 0.01f;
            if (blk[2] > 3.5f) {
                blk[1] -= 0.016666668f;
                if (blk[1] < 0.0f) {
                    self[4] = 3;
                }
            }
            break;
        }
        seed = ((int *)blk)[3];
        func_001029C0(D_700036A0);
        D_700036D0[0] = 304.0f;
        D_700036D0[1] = 210.0f;
        D_700036D0[2] = 456.0f;
        D_700036D0[3] = 1.0f;
        h = func_001CCF70(D_700036D0);
        r = (float)((seed >> 16) & 0xFFFF);
        r = r / 65535.0f;
        r += 0.0001f;
        seed = seed * 37 + 11;
        func_001CFAE0(pkt, 0, D_700036A0, blk[0], r, blk[1], 0.1f);
        func_001CFBE0(h, 1, D_overlay_AREA16_00828EF0, pkt, 1);
        func_001029C0(D_700036A0);
        D_700036D0[0] = 304.0f;
        D_700036D0[1] = 178.0f;
        D_700036D0[2] = 456.0f;
        D_700036D0[3] = 1.0f;
        h = func_001CCF70(D_700036D0);
        r = (float)((seed >> 16) & 0xFFFF);
        r = r / 65535.0f;
        r += 0.0001f;
        func_001CFAE0(pkt, 0, D_700036A0, blk[0], r, blk[1], 0.1f);
        func_001CFBE0(h, 1, D_overlay_AREA16_00828F80, pkt, 1);
        blk[0] += 0.007f;
        if (blk[0] > 2.0f) {
            blk[0] -= 1.0f;
        }
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
