// NEARMISS func_overlay_AREA21_00825AC0 (97.59%, mwcc 2.3.3; linked from its splat .s)
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA21 overlay, runtime 0x00825B00 (splat/link name 00825AC0; overlay code
//  is linked 0x40 below where it runs), 0x160 bytes.
// Role: effect object (no static reference). The AREA19 0x824A90 C with
//  func_001CFAE0(pkt, 0, +0xD0, +0x1F0, +0x1F4, 1.0, 0.1), group 0x82B0E0 and
//  a step of 1/120.
// Divergence: the func_001CFAE0 arguments: the original materialises
// 0.1 (f15) first and 1.0 (f14) after the +0x1F0 load; mwcc 2.3.3 loads 1.0
// first. Locals, int staging and argument spellings were tried.
extern char D_overlay_AREA21_0082B0E0[];
extern void func_001029C0(void *m);
extern void func_00102C58(void *dst, void *a, void *b);
extern void func_00102918(void *a, void *b, void *c);
extern int func_00122BB8(void);
extern int func_001CCF70(void *p);
extern void func_001CFAE0(void *pkt, int a1, void *m, float f12, float f13, float f14, float f15);
extern void func_001CFBE0(int h, int a1, void *a2, void *a3, int t0);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA21_00825AC0(unsigned char *self) {
    int pkt[24];
    int h;
    float *w = (float *)(self + 0x1F0);
    switch (self[4]) {
    case 0:
        func_001029C0(self + 0xD0);
        func_00102C58(self + 0xD0, self + 0xD0, self + 0xC0);
        func_00102918(self + 0xD0, self + 0xD0, self + 0xB0);
        w[0] = 0.0f;
        w[1] = (float)func_00122BB8() / 2147483648.0f;
        self[4] = 1;
    case 1:
        h = func_001CCF70(self + 0x100);
        func_001CFAE0(pkt, 0, self + 0xD0, w[0], w[1], 1.0f, 0.1f);
        func_001CFBE0(h, 0, D_overlay_AREA21_0082B0E0, pkt, 0);
        w[0] += 0.008333334f;
        if (!(w[0] <= 1.5f)) {
            self[4] = 3;
        }
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
