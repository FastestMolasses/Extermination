// COMPILER: eegcc
// CFLAGS: -O2
// SDK leaf (ee-gcc 2.9): SifDmaStat wrapper, passes &id as the status buffer.
extern int sceSifDmaStat(int chan, int *id);

int func_0010C950(int id) {
    int local = id;
    return sceSifDmaStat(-8, &local);
}
