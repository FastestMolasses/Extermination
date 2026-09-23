// COMPILER: eegcc
// CFLAGS: -O2
//
// MATCH NOTE (m3-matching lane): 45.83% under mwcc 991202 -> 100%;
// SDK-region code built with ee-gcc (as the rest of the lowmem SDK). Source unchanged apart from the compiler line.

extern int D_00241D48;
extern int D_0027A2C0;

int func_00112120(void) {
    int v1;
    if (D_00241D48 == 1) {
        v1 = (int)&D_0027A2C0 | 0x20000000;
        return *(int *)v1;
    }
    return 0;
}
