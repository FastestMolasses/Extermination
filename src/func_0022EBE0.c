// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Returns 1 when D_008101E4 == 3 or the scratchpad byte 0x70003B8D is neither
// 0 nor 4, else 0.
// Byte-matched from C with mwcc 2.3.3 (decomp lane round 3, 2026-10-20).

extern unsigned char D_008101E4[];
extern unsigned char D_70003B8D;

int func_0022EBE0(void) {
    int s;
    if (D_008101E4[0] == 3) {
        return 1;
    }
    s = D_70003B8D;
    if (s == 0 || s == 4) {
        return 0;
    }
    return 1;
}
