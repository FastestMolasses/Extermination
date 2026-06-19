// COMPILER: eegcc
// CFLAGS: -O2
extern int sceSifDmaStat(int chan, void *arg);

int func_0010C830(int a0) {
    int local = a0;
    return sceSifDmaStat(2, &local);
}
