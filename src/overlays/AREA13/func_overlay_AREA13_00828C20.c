// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA13 overlay, runtime 0x00828C60 (splat/link name 00828C20; overlay code
//  is linked 0x40 below where it runs), 0x1AC bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA13; lane A13C).
// Role: an effect (no static reach): state 0 builds its matrix from +0xC0 /
//  +0xB0; state 1 draws packet 0x82D3C0 (func_001CFB50 / func_001CFBE0) with
//  a growth step that eases from 0.1 toward 0.005, and ends (state 3) past
//  1.8. States 2/3 func_001AFC10.
extern char D_overlay_AREA13_0082D3C0[];
extern void func_001029C0(void *m);
extern void func_00102C58(void *dst, void *a, void *b);
extern void func_00102918(void *a, void *b, void *c);
extern int func_00122BB8(void);
extern int func_001CCF70(void *p);
extern void func_001CFB50(void *pkt, int a1, void *m, float f12, float f13, float f14, float f15, float f16);
extern void func_001CFBE0(int h, int a1, void *a2, void *a3, int t0);
extern void func_001AFC10(unsigned char *self);
void func_overlay_AREA13_00828C20(unsigned char *self) {
    int pkt[24];
    int h;
    float s;
    float *w = (float *)(self + 0x1F0);
    switch (self[4]) {
    case 0:
        func_001029C0(self + 0xD0);
        func_00102C58(self + 0xD0, self + 0xD0, self + 0xC0);
        func_00102918(self + 0xD0, self + 0xD0, self + 0xB0);
        w[0] = 0.0f;
        w[1] = 0.1f;
        w[2] = (float)func_00122BB8() / 2147483648.0f;
        self[4] = 1;
    case 1:
        h = func_001CCF70(self + 0x100);
        func_001CFB50(pkt, 0, self + 0xD0, w[0], w[2], 1.0f, 1e-6f, 20.0f);
        func_001CFBE0(h, 1, D_overlay_AREA13_0082D3C0, pkt, 0);
        s = w[1] + (0.005f - w[1]) / 8.0f;
        w[1] = s;
        if (s < 0.005f) {
            s = 0.005f;
        }
        w[1] = s;
        w[0] += s;
        if (!(w[0] < 1.8f)) {
            self[4] = 3;
        }
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
