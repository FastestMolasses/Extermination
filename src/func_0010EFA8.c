// COMPILER: eegcc
// CFLAGS: -O2
//
// MATCH NOTE (m3-matching lane): 51.11% under mwcc 991202 -> 100%;
// SDK-region code built with ee-gcc (as the rest of the lowmem SDK). Source unchanged apart from the compiler line.

extern char D_00279110;

int func_0010EFA8(int a0) {
    if ((unsigned int)a0 >= 0x10U) {
        return 0;
    }
    return (int)(&D_00279110 + a0 * 0x10);
}
