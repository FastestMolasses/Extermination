// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA19 overlay, runtime 0x00824EF0 (splat/link name 00824EB0; overlay code
//  is linked 0x40 below where it runs), 0x74 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA19; lane A13C).
// Role: group 0x82B5A0 member: in state 1 func_001EFEB0(0x8000004B) and
//  func_001EFEB0(4) at its matrix.
extern void func_001EFEB0(unsigned int msg, void *m);

void func_overlay_AREA19_00824EB0(unsigned char *self) {
    switch (self[4]) {
    case 0:
        break;
    case 1:
        func_001EFEB0(0x8000004B, self + 0xD0);
        func_001EFEB0(4, self + 0xD0);
        break;
    case 2:
    case 3:
        break;
    }
}
