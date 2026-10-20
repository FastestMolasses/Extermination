// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Returns 1 when D_008102B0[0] == 1 and func_0021BB00(D_008102B0) == 0, else
// 0.
// Byte-matched from C with mwcc 2.3.3 (decomp lane round 3, 2026-10-20).

extern unsigned char D_008102B0[8];
extern int func_0021BB00(unsigned char *p);

int func_0021BD10(void) {
    unsigned char *p = D_008102B0;
    if (*p == 1 && func_0021BB00(p) == 0) {
        return 1;
    }
    return 0;
}
