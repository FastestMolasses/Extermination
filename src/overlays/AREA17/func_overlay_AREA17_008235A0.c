// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA17 overlay, runtime 0x008235E0 (splat/link name 008235A0; overlay code
//  is linked 0x40 below where it runs), 0x110 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA17; lane OVLC).
// Role: effect object (no static reference). State 0: +0xD0 from
//  func_001029C0 / 00102918 (+0xB0), +0x1F0 = seed, +0x1F4 = 0, state 1.
//  State 1: func_001D04B0(+0xD0, 5, 0x8265C0 and 0x826650, +0x1F4,
//  (float)seed); +0x1F4 += 0.012, state 3 past 1.5.
extern char D_overlay_AREA17_008265C0[];
extern char D_overlay_AREA17_00826650[];
extern void func_001029C0(void *m);
extern void func_00102918(void *dst, void *src, void *v);
extern int func_00122BB8(void);
extern void func_001D04B0(void *m, int a1, void *tbl, float f12, float f13);

void func_overlay_AREA17_008235A0(unsigned char *self) {
    unsigned char *blk = self + 0x1F0;
    switch (self[4]) {
    case 0:
        func_001029C0(self + 0xD0);
        func_00102918(self + 0xD0, self + 0xD0, self + 0xB0);
        *(int *)blk = func_00122BB8();
        *(float *)(blk + 4) = 0.0f;
        self[4] = 1;
        break;
    case 1:
        func_001D04B0(self + 0xD0, 5, D_overlay_AREA17_008265C0, *(float *)(blk + 4), (float)*(int *)blk);
        func_001D04B0(self + 0xD0, 5, D_overlay_AREA17_00826650, *(float *)(blk + 4), (float)*(int *)blk);
        *(float *)(blk + 4) += 0.012f;
        if (!(*(float *)(blk + 4) <= 1.5f)) {
            self[4] = 3;
        }
        break;
    case 2:
    case 3:
        break;
    }
}
