// COMPILER: eegcc
// CFLAGS: -O2
// SDK leaf (ee-gcc 2.9): indexed slot lookup, fall back to handler if OOB.
extern int func_0010E338(int *a0, int a1);

int func_0010E368(int *a0, int a1) {
    if (a1 < 0 || a1 >= a0[0x20 / 4]) {
        return func_0010E338(a0, a1);
    }
    return a0[0x1C / 4] + (a1 << 6);
}
