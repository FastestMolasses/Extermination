// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA21 overlay, runtime 0x008262D0 (splat/link name 00826290; overlay code
//  is linked 0x40 below where it runs), 0x38 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA21; lane A03C).
// Role: sub 0 placements [43], [44]: 0x826310 when +3 is 0, else
//  0x8264D0.
extern void func_overlay_AREA21_00826310(unsigned char *self);
extern void func_overlay_AREA21_008264D0(unsigned char *self);

void func_overlay_AREA21_00826290(unsigned char *self) {
    if (self[3] == 0) {
        func_overlay_AREA21_00826310(self);
    } else {
        func_overlay_AREA21_008264D0(self);
    }
}
