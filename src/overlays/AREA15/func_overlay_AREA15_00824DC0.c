// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA15 overlay, runtime 0x00824E00 (splat/link name 00824DC0; overlay code is
// linked 0x40 below where it runs), 0x4C bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA15; lane A15C).
// Role: runs 0x824E50 for +0xD 0x59 and 0x825030 for +0xD 0x50.
extern void func_overlay_AREA15_00824E50(unsigned char *self);
extern void func_overlay_AREA15_00825030(unsigned char *self);

void func_overlay_AREA15_00824DC0(unsigned char *self) {
    if (self[0xD] == 0x59) {
        func_overlay_AREA15_00824E50(self);
    }
    if (self[0xD] == 0x50) {
        func_overlay_AREA15_00825030(self);
    }
}
