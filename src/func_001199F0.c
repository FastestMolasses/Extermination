// COMPILER: eegcc
// CFLAGS: -O2
//
// For a0 < 0x80 and 0 <= a1 < 0x80, stores a1 in byte pair a0 of D_002819C0
// (and 1 in the second byte); returns the old signed byte, or -1.
// Byte-matched from C with ee-gcc 2.9-991111-01 (decomp lane round 3, 2026-10-20).

extern signed char D_002819C0[0x100];

int func_001199F0(unsigned int a0, int a1) {
    int a2 = a1;
    int old = -1;
    if (a0 < 0x80 && a2 >= 0 && a2 < 0x80) {
        old = D_002819C0[a0 * 2];
        D_002819C0[a0 * 2] = a2;
        (D_002819C0 + 1)[a0 * 2] = 1;
    }
    return old;
}
