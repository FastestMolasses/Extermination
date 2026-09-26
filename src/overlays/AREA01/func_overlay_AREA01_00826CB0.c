// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA01 overlay, runtime 0x00826CF0 (splat/link name 00826CB0; overlay
// code is linked 0x40 below where it runs). Byte-identical (objdiff 100%).
// Role: sub-0 deferred group 0x828A00 / placement owner; +3 == 1 selects
//  func_001C5C90, else func_001C4820.
extern void func_001C5C90(unsigned char *self);
extern void func_001C4820(unsigned char *self);

void func_overlay_AREA01_00826CB0(unsigned char *self) {
    switch (self[3]) {
    case 1:
        func_001C5C90(self);
        break;
    default:
        func_001C4820(self);
        break;
    }
}
