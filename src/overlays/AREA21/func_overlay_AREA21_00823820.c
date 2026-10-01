// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA21 overlay, runtime 0x00823860 (splat/link name 00823820; overlay code
//  is linked 0x40 below where it runs), 0x78 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA21; lane A03C).
// Role: group 0x82BA70 member (x3). State 1: effect 1 at +0x100 every 30th
//  D_70003B68 frame.
extern int D_70003B68[];
extern void func_001EFD20(int id, void *pos);

void func_overlay_AREA21_00823820(unsigned char *self) {
    switch (self[4]) {
    case 0:
        break;
    case 1:
        if (D_70003B68[0] % 30 == 0) {
            func_001EFD20(1, self + 0x100);
        }
        break;
    case 2:
    case 3:
        break;
    }
}
