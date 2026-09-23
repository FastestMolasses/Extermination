// COMPILER: eegcc
// CFLAGS: -O2
//
// MATCH NOTE (m3-matching lane): 82.50% under mwcc 991202 -> 100%;
// SDK-region code built with ee-gcc (as the rest of the lowmem SDK). Source unchanged apart from the compiler line.

extern int D_0026B688[];
extern void func_00122B58(int *, int);

void func_0010A368(int a0) {
    func_00122B58(D_0026B688, a0);
}
