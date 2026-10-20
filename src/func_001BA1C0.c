// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Returns 1 when D_00810758[a1] == 0xFF, else 0.
// Byte-matched from C with mwcc 2.3.3 (decomp lane round 3, 2026-10-20).

extern unsigned char D_00810758[];

int func_001BA1C0(int a0, int a1) {
    if (D_00810758[a1] == 0xFF) {
        return 1;
    }
    return 0;
}
