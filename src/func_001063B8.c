// COMPILER: eegcc
// CFLAGS: -O2
//
// MATCH NOTE (m3-matching lane): 36.67% under mwcc 991202 -> 100%;
// SDK-region code built with ee-gcc (as the rest of the lowmem SDK). Source unchanged apart from the compiler line.

extern int D_00241170[];
extern int D_00241324;

void func_001063B8(unsigned int a0) {
    volatile int *hw = (int *)0x10002000;
    *hw = a0;
    D_00241324 = D_00241170[a0 >> 28];
}
