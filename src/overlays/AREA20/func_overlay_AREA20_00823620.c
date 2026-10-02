// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA20 overlay, runtime 0x00823660 (splat/link name 00823620; overlay code
//  is linked 0x40 below where it runs), 0x120 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA20; lane OVLC).
// Role: effect object (no static reference). State 0: +0xD0 from
//  func_001029C0 / 00102C58 (+0xC0) / 00102918 (+0xB0), +0x1F0 = 0, +0x1F4 =
//  rand fraction. State 1: func_001D04B0(+0xD0, 1, 0x826680, +0x1F0, +0x1F4);
//  +0x1F0 += 0.02, state 3 at 2.0.
extern char D_overlay_AREA20_00826680[];
extern void func_001029C0(void *m);
extern void func_00102C58(void *dst, void *src, void *v);
extern void func_00102918(void *dst, void *src, void *v);
extern int func_00122BB8(void);
extern void func_001D04B0(void *m, int a1, void *tbl, float f12, float f13);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA20_00823620(unsigned char *self) {
    unsigned char *blk = self + 0x1F0;
    switch (self[4]) {
    case 0:
        func_001029C0(self + 0xD0);
        func_00102C58(self + 0xD0, self + 0xD0, self + 0xC0);
        func_00102918(self + 0xD0, self + 0xD0, self + 0xB0);
        *(float *)blk = 0.0f;
        *(float *)(blk + 4) = (float)func_00122BB8() / 2147483648.0f;
        self[4] = 1;
        break;
    case 1:
        func_001D04B0(self + 0xD0, 1, D_overlay_AREA20_00826680, *(float *)blk, *(float *)(blk + 4));
        *(float *)blk += 0.02f;
        if (!(*(float *)blk < 2.0f)) {
            self[4] = 3;
        }
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
