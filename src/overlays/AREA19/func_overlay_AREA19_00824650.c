// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA19 overlay, runtime 0x00824690 (splat/link name 00824650; overlay code
//  is linked 0x40 below where it runs), 0x3F4 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA19; lane A13C).
// Role: sub 0 placement [11]: a flame at the object matrix: packet 0x82B1B0
//  at a point y -110 below it (-120 with D_00810775 bit 0; 0x82B234 = that +
//  5), a second packet 0x82B240, random func_001E8B90 sparks (half the
//  frames) and func_001FC3C0(self, blk + 0x10, 0x419, 300, 4096). States 2/3
//  func_001FC520 and func_001AFC10.
typedef struct { float x, y, z, w; } Vec4;
extern unsigned char D_00810775;
extern float D_700036A0[];
extern float D_700036D0[];
extern float D_700038A0[];
extern char D_overlay_AREA19_0082B1B0[];
extern float D_overlay_AREA19_0082B234;
extern char D_overlay_AREA19_0082B240[];
extern void func_001029C0(void *m);
extern void func_00102C58(void *dst, void *a, void *b);
extern void func_00102918(void *a, void *b, void *c);
extern int func_00122BB8(void);
extern void func_001026A0(void *dst, void *m, void *v);
extern void func_001CFA60(void *pkt, void *m, float f12, float f13);
extern void func_001CFBE0(int h, int a1, void *a2, void *a3, int t0);
extern int func_001CCF70(void *p);
extern void func_00102948(void *dst, void *src);
extern void func_001E8B90(Vec4 *v, float s);
extern void func_001FC3C0(unsigned char *self, void *h, int id, float vol, float f13);
extern void func_001FC520(void *h);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA19_00824650(unsigned char *self) {
    int pkt[24];
    Vec4 v;
    unsigned char *blk = self + 0x1F0;
    float y;
    int h;
    int zi;
    float z;
    switch (self[4]) {
    case 0:
        func_001029C0(self + 0xD0);
        func_00102C58(self + 0xD0, self + 0xD0, self + 0xC0);
        func_00102918(self + 0xD0, self + 0xD0, self + 0xB0);
        *(int *)(blk + 0x10) = -1;
        *(float *)(blk + 0) = 0.0f;
        *(float *)(blk + 4) = 0.0f;
        *(float *)(blk + 8) = (float)func_00122BB8() / 2147483648.0f;
        *(float *)(blk + 0xC) = (float)func_00122BB8() / 2147483648.0f;
        self[4] = 1;
    case 1:
        if (D_00810775 & 1) {
            y = -120.0f;
        } else {
            y = -110.0f;
        }
        D_700038A0[0] = 0.0f;
        *(float *)0x700038A4 = y;
        *(float *)0x700038A8 = 30.0f;
        *(float *)0x700038AC = 1.0f;
        func_001026A0(D_700038A0, self + 0xD0, D_700038A0);
        D_overlay_AREA19_0082B234 = 5.0f + y;
        func_001CFA60(pkt, self + 0xD0, *(float *)(blk + 0), *(float *)(blk + 8));
        func_001CFBE0(0xFFB000, 1, D_overlay_AREA19_0082B1B0, pkt, 1);
        *(float *)(blk + 0) += 0.015f;
        if (!(*(float *)(blk + 0) < 2.0f)) {
            *(float *)(blk + 0) -= 1.0f;
        }
        func_001029C0(D_700036A0);
        func_00102918(D_700036A0, D_700036A0, D_700038A0);
        h = func_001CCF70(D_700036D0);
        func_001CFA60(pkt, D_700036A0, *(float *)(blk + 4), *(float *)(blk + 0xC));
        func_001CFBE0(h, 1, D_overlay_AREA19_0082B240, pkt, 0);
        *(float *)(blk + 4) += 0.02f;
        if (!(*(float *)(blk + 4) < 2.0f)) {
            *(float *)(blk + 4) -= 1.0f;
        }
        if (!(4.656613e-10f * (float)func_00122BB8() <= 0.5f)) {
            func_00102948(&v, D_700036D0);
            zi = 0;
            z = (float)zi;
            v.x += -10.0f + 20.0f * (4.656613e-10f * (float)func_00122BB8());
            v.z += -10.0f + 20.0f * (4.656613e-10f * (float)func_00122BB8());
            func_001E8B90(&v, z + 0.8f * (4.656613e-10f * (float)func_00122BB8()));
        }
        func_001FC3C0(self, blk + 0x10, 0x419, 300.0f, 4096.0f);
        break;
    case 2:
    case 3:
        func_001FC520(blk + 0x10);
        func_001AFC10(self);
        break;
    }
}
