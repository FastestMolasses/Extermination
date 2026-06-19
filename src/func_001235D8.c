// COMPILER: eegcc
// CFLAGS: -O2
// SDK leaf (ee-gcc 2.9): if armed, flush via helper; clear state fields.
extern int func_001205D8(int a0, int *p);

int func_001235D8(int a0, int *p) {
    int r;
    if (p[2] == 0) {
        p[1] = 0;
        return 0;
    }
    r = func_001205D8(a0, p);
    p[2] = 0;
    p[1] = 0;
    return r;
}
