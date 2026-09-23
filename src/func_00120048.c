// COMPILER: eegcc
// CFLAGS: -O2
//
// MATCH NOTE (m3-matching lane): 96.67% under mwcc 991202 -> 100%;
// SDK-region code built with ee-gcc (as the rest of the lowmem SDK). Source unchanged apart from the compiler line.

extern int D_0024295C;
extern void func_00120038(int);

void func_00120048(void) {
    int a0 = D_0024295C;
    func_00120038(a0);
}
