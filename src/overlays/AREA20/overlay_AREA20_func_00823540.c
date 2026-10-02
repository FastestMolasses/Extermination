// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA20 overlay, runtime 0x00823580 (splat/link name 00823540; overlay code
//  is linked 0x40 below where it runs), 0x74 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA20; lane OVLC).
// Role: effect object (no static reference): state 1 effect 0 at +0x100
//  (func_001EFD20) and effect 0x80000053 on +0xD0 (func_001EFEB0) every
//  frame.
extern void func_001EFD20(int id, void *pos);
extern void *func_001EFEB0(int id, void *m);

void overlay_AREA20_func_00823540(unsigned char *self) {
    switch (self[4]) {
    case 0:
        break;
    case 1:
        func_001EFD20(0, self + 0x100);
        func_001EFEB0(0x80000053, self + 0xD0);
        break;
    case 2:
    case 3:
        break;
    }
}
