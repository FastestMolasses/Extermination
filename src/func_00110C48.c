// COMPILER: eegcc
// CFLAGS: -O2
//
// As func_00110BC0 for the 4-entry string table D_00241CD8.
// Byte-matched from C with ee-gcc 2.9-991111-01 (decomp lane round 3, 2026-10-20).

extern char *D_00241CD8[4];
extern char D_0026BAD0[];
extern void func_00123168(char *dst, char *src);

void func_00110C48(unsigned int a0, char *a1) {
    if (a0 < 4) {
        func_00123168(a1, D_00241CD8[a0]);
        return;
    }
    *a1 = D_0026BAD0[0];
}
