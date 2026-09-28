// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA04 overlay, runtime 0x008239A0 (splat/link name 00823960;
// overlay code is linked 0x40 below where it runs), 0xF0 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA04; lane A04C).
// Role: state 0 sets +0x1F0 = 0 and +0x1F4 = a random fraction; state 1
// calls func_001D04B0(self + 0xD0, 1, 0x8274A0, t, phase) and adds 0.01 to
// t, going to state 3 above 2.0; states 2/3 func_001AFC10 (the AREA00
// 0x823CF0 shape without the matrix setup).
extern char D_overlay_AREA04_008274A0[];
extern int func_00122BB8(void);
extern void func_001D04B0(void *m, int a1, void *a2, float f12, float f13);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA04_00823960(unsigned char *self) {
    float *w = (float *)(self + 0x1F0);
    switch (self[4]) {
    case 0:
        w[0] = 0.0f;
        w[1] = (float)func_00122BB8() / 2147483648.0f;
        self[4] = 1;
    case 1:
        func_001D04B0(self + 0xD0, 1, D_overlay_AREA04_008274A0, w[0], w[1]);
        w[0] += 0.01f;
        if (!(w[0] <= 2.0f)) {
            self[4] = 3;
        }
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
