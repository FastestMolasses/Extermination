// COMPILER: eegcc
// CFLAGS: -O2
// SDK wrapper: sceSifDmaStat(0x10, &local) where local = a0.
extern int sceSifDmaStat(int a0, int *p);

int func_0010C9A0(int a0) {
    int local = a0;
    return sceSifDmaStat(0x10, &local);
}
