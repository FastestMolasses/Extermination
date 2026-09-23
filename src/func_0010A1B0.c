// COMPILER: eegcc
// CFLAGS: -O2
//
// MATCH NOTE (m3-matching lane): 97.50% under mwcc 991202 -> 100%;
// SDK-region code built with ee-gcc (as the rest of the lowmem SDK). Source unchanged apart from the compiler line.

extern int D_00241BB0;
extern void func_00105148(int);

void func_0010A1B0(void) {
    D_00241BB0 = 0;
    func_00105148(1);
}
