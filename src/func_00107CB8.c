// COMPILER: eegcc
// CFLAGS: -O2
//
// Sets field +8 of the record at *(D_002412F4 + 0x40) to 2 (copying D_00241404
// into +0xAC when it changes), sets D_00241B80 = 1 and returns 1.
// Byte-matched from C with ee-gcc 2.9-991111-01 (decomp lane round 3, 2026-10-20).

extern char *D_002412F4;
extern int D_00241404;
extern int D_00241B80;

int func_00107CB8(void) {
    int *p = *(int **)(D_002412F4 + 0x40);
    if (p[2] != 2) {
        p[0x2B] = D_00241404;
        p[2] = 2;
    }
    D_00241B80 = 1;
    return 1;
}
