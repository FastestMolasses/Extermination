// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA21 overlay, runtime 0x00824CF0 (splat/link name 00824CB0; overlay code
//  is linked 0x40 below where it runs), 0x2F0 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA21; lane A03C).
// Role: effect object (no static reference in the tables
//  area_overview.py reads). State 0 builds the +0xD0 matrix (func_001029C0 /
//  00102C58 / 00102918), +0x1FC = 0, +0x1F8 = func_00122BB8(), +0x1F0 = 0,
//  +0x1F4 = 1, state 1. State 1 draws func_001CFB50 packets queued by
//  func_001CFBE0 on func_001CCF70(+0x100) by +0xD: 3 / 5: groups 0x82AE50 and
//  0x82AEE0, +0x1F0 += 0.03, state 3 above 1.2; 4: group 0x82AF70 with
//  +0x1F4 as the third value, +0x1F0 += 0.01 wrapping above 2.0, and after
//  240 frames +0x1F4 falls by 1/60 a frame, state 3 below 0. The packet
//  constants are int-staged (idiom-24 family).
extern char D_overlay_AREA21_0082AE50[];
extern char D_overlay_AREA21_0082AEE0[];
extern char D_overlay_AREA21_0082AF70[];
extern void func_001029C0(void *m);
extern void func_00102C58(void *dst, void *a, void *b);
extern void func_00102918(void *a, void *b, void *c);
extern int func_00122BB8(void);
extern int func_001CCF70(void *p);
extern void func_001CFB50(void *p, int a1, void *a2, float f12, float f13, float f14, float f15, float f16);
extern void func_001CFBE0(int h, int a1, void *a2, void *a3, int t0);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA21_00824CB0(unsigned char *self) {
    int h;
    float *w = (float *)(self + 0x1F0);
    int pkt[24];
    int seed;
    float r;
    switch (self[4]) {
    case 0:
        func_001029C0(self + 0xD0);
        func_00102C58(self + 0xD0, self + 0xD0, self + 0xC0);
        func_00102918(self + 0xD0, self + 0xD0, self + 0xB0);
        ((int *)w)[3] = 0;
        ((int *)w)[2] = func_00122BB8();
        w[0] = 0.0f;
        w[1] = 1.0f;
        self[4] = 1;
    case 1:
        h = func_001CCF70(self + 0x100);
        seed = ((int *)w)[2];
        switch (self[0xD]) {
        case 3:
        case 5:
            r = (float)((seed >> 16) & 0xFFFF);
            r = r / 65535.0f;
            r += 0.0001f;
            { int j = 1; int k = 5; float s1 = (float)j; float s5 = (float)k; func_001CFB50(pkt, 0, self + 0xD0, w[0], r, s1, 1e-6f, s5); }
            func_001CFBE0(h, 1, D_overlay_AREA21_0082AE50, pkt, 0);
            func_001CFBE0(h, 1, D_overlay_AREA21_0082AEE0, pkt, 0);
            w[0] += 0.03f;
            if (!(w[0] <= 1.2f)) {
                self[4] = 3;
            }
            break;
        case 4:
            r = (float)((seed >> 16) & 0xFFFF);
            r = r / 65535.0f;
            r += 0.0001f;
            func_001CFB50(pkt, 0, self + 0xD0, w[0], r, w[1], 1e-6f, 5.0f);
            func_001CFBE0(h, 1, D_overlay_AREA21_0082AF70, pkt, 0);
            w[0] += 0.01f;
            if (!(w[0] <= 2.0f)) {
                w[0] -= 1.0f;
            }
            ((int *)w)[3]++;
            if (((int *)w)[3] > 0xF0) {
                w[1] -= 0.016666668f;
                if (w[1] < 0.0f) {
                    self[4] = 3;
                }
            }
            break;
        }
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
