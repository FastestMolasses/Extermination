// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA15 overlay, runtime 0x008252D0 (splat/link name 00825290; overlay code is
// linked 0x40 below where it runs), 0x44 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA15; lane A15C).
// Role: +3 == 1 selects func_001C5C90, else func_001C4820.
extern void func_001C5C90(unsigned char *self);
extern void func_001C4820(unsigned char *self);

void func_overlay_AREA15_00825290(unsigned char *self) {
    switch (self[3]) {
    case 1:
        func_001C5C90(self);
        break;
    default:
        func_001C4820(self);
        break;
    }
}
