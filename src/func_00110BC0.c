// COMPILER: eegcc
// CFLAGS: -O2
//
// Copies name a0 of the 8-entry string table D_00241CB8 into a1 (func_00123168
// is the string copy), or the first byte of D_0026BAD0 for a0 >= 8.
// Byte-matched from C with ee-gcc 2.9-991111-01 (decomp lane round 3, 2026-10-20).

extern char *D_00241CB8[8];
extern char D_0026BAD0[];
extern void func_00123168(char *dst, char *src);

void func_00110BC0(unsigned int a0, char *a1) {
    if (a0 < 8) {
        func_00123168(a1, D_00241CB8[a0]);
        return;
    }
    *a1 = D_0026BAD0[0];
}
