// COMPILER: eegcc
// CFLAGS: -O2
//
// MATCH NOTE (m3-matching lane): 78.89% under mwcc 991202 -> 100%;
// SDK-region code built with ee-gcc (as the rest of the lowmem SDK). Source unchanged apart from the compiler line.

extern char D_0026B2B8[4];
extern void func_00122B58(char *);

int func_00102678(void) {
    func_00122B58(D_0026B2B8);
    return 0;
}
