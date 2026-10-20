// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Returns 0 when D_00810700 == 8, D_00810701 == 2 and D_00810770 == 0xFF, else
// 1.
// Byte-matched from C with mwcc 2.3.3 (decomp lane round 3, 2026-10-20).

extern unsigned char D_00810700[];
extern unsigned char D_00810701[];
extern unsigned char D_00810770[];

int func_0021C3F0(void) {
    if (D_00810700[0] == 8 && D_00810701[0] == 2 && D_00810770[0] == 0xFF) {
        return 0;
    }
    return 1;
}
