// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA00 overlay, runtime 0x00824130 (splat/link name 008240F0; overlay
// code is linked 0x40 below where it runs). Byte-identical (objdiff 100%,
// tools/overlay/overlay_match.py check AREA00).
// Role: in state 1 calls func_001EFEB0(0x80000050, self + 0xD0) and
//  func_001EFD20(0x80000015, self + 0x100).
extern void func_001EFEB0(unsigned int id, void *m);
extern void func_001EFD20(int a, void *b);

void func_overlay_AREA00_008240F0(unsigned char *self) {
    switch (self[4]) {
    case 0:
        break;
    case 1:
        func_001EFEB0(0x80000050, self + 0xD0);
        func_001EFD20(0x80000015, self + 0x100);
        break;
    case 2:
    case 3:
        break;
    }
}
