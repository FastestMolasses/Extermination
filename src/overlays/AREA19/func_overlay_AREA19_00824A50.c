// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA19 overlay, runtime 0x00824A90 (splat/link name 00824A50; overlay code
//  is linked 0x40 below where it runs), 0x148 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA19; lane A13C).
// Role: an effect (no static reach): matrix from +0xC0 / +0xB0 and a random
//  phase; packet 0x82B2D0 (func_001CFA60 / func_001CFBE0), t += 0.02, state 3
//  past 1.5.
extern char D_overlay_AREA19_0082B2D0[];
extern void func_001029C0(void *m);
extern void func_00102C58(void *dst, void *a, void *b);
extern void func_00102918(void *a, void *b, void *c);
extern int func_00122BB8(void);
extern int func_001CCF70(void *p);
extern void func_001CFA60(void *pkt, void *m, float f12, float f13);
extern void func_001CFBE0(int h, int a1, void *a2, void *a3, int t0);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA19_00824A50(unsigned char *self) {
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
        func_001CFA60(pkt, self + 0xD0, w[0], w[1]);
        func_001CFBE0(h, 1, D_overlay_AREA19_0082B2D0, pkt, 0);
        w[0] += 0.02f;
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
