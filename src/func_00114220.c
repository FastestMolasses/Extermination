// COMPILER: eegcc
// CFLAGS: -O2
//
// MATCH NOTE (m3-matching lane): 54.44% under mwcc 991202 -> 100%;
// SDK-region code built with ee-gcc (as the rest of the lowmem SDK). Source unchanged apart from the compiler line.

extern int D_0027C680;
extern int D_00241D68;
extern int D_0027B0C0;

int *func_00114220(int **a0, int **a1) {
    *a0 = &D_0027C680;
    *a1 = &D_00241D68;
    return &D_0027B0C0;
}
