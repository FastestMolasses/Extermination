// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA20 overlay, runtime 0x00823840 (splat/link name 00823800; overlay code
//  is linked 0x40 below where it runs), 0x14C bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA20; lane OVLC).
// Role: effect object (no static reference). State 0: +0x1F0 = 0, +0x1F4 =
//  rand fraction. State 1: func_001D04B0(+0xD0, 1, 0x826710 for +0x94 == 11,
//  0x8267A0 for 10 / 12, +0x1F0, +0x1F4); +0x1F0 += 0.03, state 3 at 1.2.
extern char D_overlay_AREA20_00826710[];
extern char D_overlay_AREA20_008267A0[];
extern int func_00122BB8(void);
extern void func_001D04B0(void *m, int a1, void *tbl, float f12, float f13);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA20_00823800(unsigned char *self) {
    unsigned char *blk = self + 0x1F0;
    switch (self[4]) {
    case 0:
        *(float *)blk = 0.0f;
        *(float *)(blk + 4) = (float)func_00122BB8() / 2147483648.0f;
        self[4] = 1;
        break;
    case 1:
        switch (*(short *)(self + 0x94)) {
        case 11:
            func_001D04B0(self + 0xD0, 1, D_overlay_AREA20_00826710, *(float *)blk, *(float *)(blk + 4));
            break;
        case 10:
        case 12:
            func_001D04B0(self + 0xD0, 1, D_overlay_AREA20_008267A0, *(float *)blk, *(float *)(blk + 4));
            break;
        }
        *(float *)blk += 0.03f;
        if (!(*(float *)blk < 1.2f)) {
            self[4] = 3;
        }
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
