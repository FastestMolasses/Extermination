// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA15 overlay, runtime 0x00824510 (splat/link name 008244D0; overlay code is
// linked 0x40 below where it runs), 0x4C bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA15; lane A15C).
// Role: runs 0x824560 for +0xD 0x64 and 0x8247B0 for +0xD 0x50.
extern void func_overlay_AREA15_00824560(unsigned char *self);
extern void func_overlay_AREA15_008247B0(unsigned char *self);

void func_overlay_AREA15_008244D0(unsigned char *self) {
    if (self[0xD] == 0x64) {
        func_overlay_AREA15_00824560(self);
    }
    if (self[0xD] == 0x50) {
        func_overlay_AREA15_008247B0(self);
    }
}
