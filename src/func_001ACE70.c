// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Returns 3 when D_008106CE is set, else 2 when D_008106C5 or D_008106B0 is
// set, else 0.
// Byte-matched from C with mwcc 2.3.3 (decomp lane round 3, 2026-10-20).

extern unsigned char D_008106CE[];
extern unsigned char D_008106C5[];
extern unsigned char D_008106B0[];

int func_001ACE70(void) {
    if (D_008106CE[0] != 0) {
        return 3;
    }
    if (D_008106C5[0] != 0 || D_008106B0[0] != 0) {
        return 2;
    }
    return 0;
}
