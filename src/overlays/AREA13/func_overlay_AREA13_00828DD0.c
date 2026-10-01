// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA13 overlay, runtime 0x00828E10 (splat/link name 00828DD0; overlay code
//  is linked 0x40 below where it runs), 0x11C bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA13; lane A13C).
// Role: an effect (no static reach): state 0 builds its matrix and a random
//  phase; state 1 calls func_001D04B0(matrix, 5, 0x82D450, t, phase), t +=
//  0.008, state 3 past 1.5.
extern char D_overlay_AREA13_0082D450[];
extern void func_001029C0(void *m);
extern void func_00102C58(void *dst, void *a, void *b);
extern void func_00102918(void *a, void *b, void *c);
extern int func_00122BB8(void);
extern void func_001D04B0(void *m, int a1, void *a2, float f12, float f13);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA13_00828DD0(unsigned char *self) {
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
        func_001D04B0(self + 0xD0, 5, D_overlay_AREA13_0082D450, w[0], w[1]);
        w[0] += 0.008f;
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
