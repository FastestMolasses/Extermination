// COMPILER: eegcc
// CFLAGS: -O2
extern int sceSifDmaStat(int chan, void *arg);

int func_0010C858(int a0, signed char a1) {
    int local[2];
    local[0] = a0;
    local[1] = a1;
    return sceSifDmaStat(3, local);
}
