// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA13 overlay, runtime 0x008284E0 (splat/link name 008284A0; overlay code
//  is linked 0x40 below where it runs), 0x20 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA13; lane A13C).
// Role: collision method stored by 0x828500 (+0x34 = 0x8284E0): state 2
//  unless the other object's +0 bit 1 is set.
void func_overlay_AREA13_008284A0(unsigned char *self, unsigned char *other) {
    if (!(other[0] & 2)) {
        self[4] = 2;
    }
}
