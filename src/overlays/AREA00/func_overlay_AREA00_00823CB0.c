// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA00 overlay, runtime 0x00823CF0 (splat/link name 00823CB0; overlay
// code is linked 0x40 below where it runs). Byte-identical (objdiff 100%,
// tools/overlay/overlay_match.py check AREA00).
// Role: state 0 builds the +0xD0 matrix from +0xC0 and +0xB0, sets
//  +0x1F0 = 0 and +0x1F4 = a random fraction; state 1 calls
//  func_001D04B0(self + 0xD0, 1, 0x828900, t, phase) and adds 0.008 to t,
//  going to state 3 once t passes 1.5; states 2 and 3 call func_001AFC10.
extern char D_overlay_AREA00_00828900[];
extern void func_001029C0(void *m);
extern void func_00102C58(void *dst, void *a, void *b);
extern void func_00102918(void *a, void *b, void *c);
extern int func_00122BB8(void);
extern void func_001D04B0(void *m, int a1, void *a2, float f12, float f13);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA00_00823CB0(unsigned char *self) {
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
        func_001D04B0(self + 0xD0, 1, D_overlay_AREA00_00828900, w[0], w[1]);
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
