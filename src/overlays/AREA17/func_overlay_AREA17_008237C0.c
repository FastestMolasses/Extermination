// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA17 overlay, runtime 0x00823800 (splat/link name 008237C0; overlay code
//  is linked 0x40 below where it runs), 0x58 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA17; lane OVLC).
// Role: effect object (no static reference): state 1 effect 1 at +0x100
//  (func_001EFD20) every frame.
extern void func_001EFD20(int id, void *pos);

void func_overlay_AREA17_008237C0(unsigned char *self) {
    switch (self[4]) {
    case 0:
        break;
    case 1:
        func_001EFD20(1, self + 0x100);
        break;
    case 2:
    case 3:
        break;
    }
}
