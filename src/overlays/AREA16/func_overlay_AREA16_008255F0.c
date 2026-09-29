// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA16 overlay, runtime 0x00825630 (splat/link name 008255F0; overlay code is
// linked 0x40 below where it runs), 0x24 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA16; lane A15C).
// Role: (self, a1, a2) callback: +0x2E = 0xFFFF when a2[2] != 0, else 0;
//  returns 1.
int func_overlay_AREA16_008255F0(unsigned char *self, unsigned char *a1, int *a2) {
    if (a2[2] != 0) {
        *(unsigned short *)(self + 0x2E) = 0xFFFF;
    } else {
        *(unsigned short *)(self + 0x2E) = 0;
    }
    return 1;
}
