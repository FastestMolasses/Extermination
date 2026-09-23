// COMPILER: eegcc
// CFLAGS: -O2
//
// MATCH NOTE (m3-matching lane): 96.00% under mwcc 991202 -> 100%;
// SDK-region code built with ee-gcc (as the rest of the lowmem SDK). Source unchanged apart from the compiler line.

extern int *D_002412F4;

void func_00109BC0(int a0) {
    int *v1 = D_002412F4;
    int *a1 = (int *)*(int *)((char *)v1 + 0x40);
    *(int *)((char *)a1 + 0xD8) = a0;
}
