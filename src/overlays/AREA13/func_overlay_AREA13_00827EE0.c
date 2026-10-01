// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// AREA13 overlay, runtime 0x00827F20 (splat/link name 00827EE0; overlay code
//  is linked 0x40 below where it runs), 0x70 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA13; lane A13C).
// Role: step 3 of 0x827150: func_001C64F0(self, 1.0) while the count is above
//  0xB4, func_001F02C0(+0xB0, 0x8D4, 500) at count 0; limit 0x168.
extern int *D_00275CA8;
extern void func_001C64F0(unsigned char *self, float step);
extern void func_001F02C0(void *pos, int id, float vol);

void func_overlay_AREA13_00827EE0(unsigned char *self) {
    if (D_00275CA8[1] > 0xB4) {
        func_001C64F0(self, 1.0f);
    }
    if (D_00275CA8[1] == 0) {
        func_001F02C0(self + 0xB0, 0x8D4, 500.0f);
    }
    D_00275CA8[2] = 0x168;
}
