// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA17 overlay, runtime 0x008236F0 (splat/link name 008236B0; overlay code
//  is linked 0x40 below where it runs), 0x110 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA17; lane OVLC).
// Role: effect object (no static reference). State 0: +0x1F0 = seed, +0x1F4 =
//  0.8. State 1: effect 1 at +0x100 (func_001EFD20) while +0x1F4 is exactly
//  0.8; func_001D04B0(+0xD0, 5, 0x8266E0, +0x1F4, (float)seed); +0x1F4 +=
//  0.001, wrapping above 2.0 by -1.
extern char D_overlay_AREA17_008266E0[];
extern int func_00122BB8(void);
extern void func_001EFD20(int id, void *pos);
extern void func_001D04B0(void *m, int a1, void *tbl, float f12, float f13);

void func_overlay_AREA17_008236B0(unsigned char *self) {
    unsigned char *blk = self + 0x1F0;
    switch (self[4]) {
    case 0:
        *(int *)blk = func_00122BB8();
        *(float *)(blk + 4) = 0.8f;
        break;
    case 1:
        if (*(float *)(blk + 4) == 0.8f) {
            func_001EFD20(1, self + 0x100);
        }
        func_001D04B0(self + 0xD0, 5, D_overlay_AREA17_008266E0, *(float *)(blk + 4), (float)*(int *)blk);
        *(float *)(blk + 4) += 0.001f;
        if (!(*(float *)(blk + 4) <= 2.0f)) {
            *(float *)(blk + 4) -= 1.0f;
        }
        break;
    case 2:
    case 3:
        break;
    }
}
