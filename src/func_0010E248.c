// COMPILER: eegcc
// CFLAGS: -O2
//
// MATCH NOTE (m3-matching lane): 70.00% under mwcc 991202 -> 100%;
// SDK-region code built with ee-gcc (as the rest of the lowmem SDK). Source unchanged apart from the compiler line.

extern int D_00241C80;
extern void func_0010DC40(void);

void func_0010E248(void) {
    func_0010DC40();
    D_00241C80 = 0;
}
