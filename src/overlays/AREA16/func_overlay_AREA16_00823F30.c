// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA16 overlay, runtime 0x00823F70 (splat/link name 00823F30; overlay code is
// linked 0x40 below where it runs), 0x1A0 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA16; lane A15C).
// Role: state 0: builds the self + 0xD0 matrix, +0x1F0 = 0, +0x1F4 = rand
//  seed, state 1 (falls through). State 1: two func_001D04B0 draws (tables
//  0x829190 and 0x829220) with seeded random widths, +0x1F0 += 0.02, state 3
//  past 1.2. States 2/3 func_001AFC10.
extern char D_overlay_AREA16_00829190[];
extern char D_overlay_AREA16_00829220[];
extern void func_001029C0(void *m);
extern void func_00102C58(void *dst, void *src, void *rot);
extern void func_00102918(void *dst, void *src, void *pos);
extern int func_00122BB8(void);
extern void func_001D04B0(void *m, int a1, void *tbl, float f12, float f13);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA16_00823F30(unsigned char *self) {
    float *blk = (float *)(self + 0x1F0);
    int seed;
    float r;
    switch (self[4]) {
    case 0:
        func_001029C0(self + 0xD0);
        func_00102C58(self + 0xD0, self + 0xD0, self + 0xC0);
        func_00102918(self + 0xD0, self + 0xD0, self + 0xB0);
        blk[0] = 0.0f;
        ((int *)blk)[1] = func_00122BB8();
        self[4] = 1;
    case 1:
        seed = ((int *)blk)[1];
        r = (float)((seed >> 16) & 0xFFFF);
        r = r / 65535.0f;
        r += 0.0001f;
        seed = seed * 37 + 11;
        func_001D04B0(self + 0xD0, 1, D_overlay_AREA16_00829190, blk[0], r);
        r = (float)((seed >> 16) & 0xFFFF);
        r = r / 65535.0f;
        r += 0.0001f;
        func_001D04B0(self + 0xD0, 1, D_overlay_AREA16_00829220, blk[0], r);
        blk[0] += 0.02f;
        if (blk[0] > 1.2f) {
            self[4] = 3;
        }
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
