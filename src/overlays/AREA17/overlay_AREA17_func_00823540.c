// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA17 overlay, runtime 0x00823580 (splat/link name 00823540; overlay code
//  is linked 0x40 below where it runs), 0x58 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA17; lane OVLC).
// Role: effect object (no static reference): state 1 effect 0 at +0x100
//  (func_001EFD20) every frame; the AREA20 0x823600 C.
extern void func_001EFD20(int id, void *pos);

void overlay_AREA17_func_00823540(unsigned char *self) {
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
