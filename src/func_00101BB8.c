// COMPILER: eegcc
// CFLAGS: -O2
// SDK leaf (ee-gcc 2.9): bounds-checked table lookup.
extern int D_00241050[];

int func_00101BB8(int a0) {
    if ((unsigned int)a0 < 10) {
        return D_00241050[a0];
    }
    return 0;
}
