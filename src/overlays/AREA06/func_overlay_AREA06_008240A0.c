// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA06 overlay, runtime 0x008240E0 (splat/link name 008240A0; overlay code is
// linked 0x40 below where it runs), 0x1A4 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA06; lane A06C).
// Role: state 0 sets +0x1F0 = 0 and +0x1F4/+0x1F8 to random fractions; state
// 1 calls func_001D04B0(self + 0xD0, n, table, t, phase) per variant +0xD
// (0: table 0x826B00; 1: 0x826B90; 2: 0x826C20 then 0x826CB0 with the second
// phase), adds 0.015 to t and goes to state 3 once t is not below 1.1; states
// 2/3 func_001AFC10 (the AREA04 0x8239A0 shape).
extern char D_overlay_AREA06_00826B00[];
extern char D_overlay_AREA06_00826B90[];
extern char D_overlay_AREA06_00826C20[];
extern char D_overlay_AREA06_00826CB0[];
extern int func_00122BB8(void);
extern void func_001D04B0(void *m, int a1, void *a2, float f12, float f13);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA06_008240A0(unsigned char *self) {
    float *w = (float *)(self + 0x1F0);
    switch (self[4]) {
    case 0:
        w[0] = 0.0f;
        w[1] = (float)func_00122BB8() / 2147483648.0f;
        w[2] = (float)func_00122BB8() / 2147483648.0f;
        self[4] = 1;
    case 1:
        switch (self[0xD]) {
        case 0:
            func_001D04B0(self + 0xD0, 2, D_overlay_AREA06_00826B00, w[0], w[1]);
            break;
        case 1:
            func_001D04B0(self + 0xD0, 2, D_overlay_AREA06_00826B90, w[0], w[1]);
            break;
        case 2:
            func_001D04B0(self + 0xD0, 2, D_overlay_AREA06_00826C20, w[0], w[1]);
            func_001D04B0(self + 0xD0, 1, D_overlay_AREA06_00826CB0, w[0], w[2]);
            break;
        }
        w[0] += 0.015f;
        if (!(w[0] < 1.1f)) {
            self[4] = 3;
        }
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
