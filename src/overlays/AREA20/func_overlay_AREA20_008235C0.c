// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA20 overlay, runtime 0x00823600 (splat/link name 008235C0; overlay code
//  is linked 0x40 below where it runs), 0x58 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA20; lane OVLC).
// Role: effect object (no static reference): state 1 effect 0 at +0x100
//  (func_001EFD20) every frame.
extern void func_001EFD20(int id, void *pos);

void func_overlay_AREA20_008235C0(unsigned char *self) {
    switch (self[4]) {
    case 0:
        break;
    case 1:
        func_001EFD20(0, self + 0x100);
        break;
    case 2:
    case 3:
        break;
    }
}
