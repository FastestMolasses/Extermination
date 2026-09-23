// COMPILER: eegcc
// CFLAGS: -O2
//
// MATCH NOTE (m3-matching lane): 80.00% under mwcc 991202 -> 100%;
// SDK-region code built with ee-gcc (as the rest of the lowmem SDK). Source unchanged apart from the compiler line.

extern int D_00277340[];

int func_0010D970(int a0, int a1) {
    D_00277340[a0] = a1;
    return a1;
}
