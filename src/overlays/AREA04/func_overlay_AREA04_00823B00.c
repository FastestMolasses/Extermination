// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA04 overlay, runtime 0x00823B40 (splat/link name 00823B00;
// overlay code is linked 0x40 below where it runs), 0x4C bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA04; lane A04C).
// Role: once (D_00810845 bit 3 clear): sets the bit and calls
// func_001FB9F0(0x3EE, 0x1000, 0x1000, 0x1000); returns 1.
extern unsigned char D_00810845;
extern void func_001FB9F0(int id, int a, int b, int c);

int func_overlay_AREA04_00823B00(void) {
    if (!(D_00810845 & 8)) {
        D_00810845 |= 8;
        func_001FB9F0(0x3EE, 0x1000, 0x1000, 0x1000);
    }
    return 1;
}
