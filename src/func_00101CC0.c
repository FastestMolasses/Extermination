// COMPILER: eegcc
// CFLAGS: -O2
//
// MATCH NOTE (m3-matching lane): 71.25% under mwcc 991202 -> 100%;
// SDK-region code built with ee-gcc (as the rest of the lowmem SDK). Source unchanged apart from the compiler line.

extern char D_00241078[];

int func_00101CC0(int a0) {
    int *v1 = (int *)D_00241078;
    int v0 = *v1;
    *v1 = a0;
    return v0;
}
