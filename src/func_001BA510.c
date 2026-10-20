// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Clears the 12 bytes D_008106B0 + 0x24 .. + 0x2F.
// Byte-matched from C with mwcc 2.3.3 (decomp lane round 3, 2026-10-20).

extern char D_008106B0[];
void func_001BA510(void) {
    int i;
    for (i = 0; i < 12; i++) {
        D_008106B0[0x24 + i] = 0;
    }
}
