// COMPILER: eegcc
// CFLAGS: -O2
extern int sceSifDmaStat(int chan, void *arg);

int func_0010C8E8(int a0, int a1, unsigned short a2) {
    int local[3];
    local[0] = a0;
    local[1] = a1;
    local[2] = a2;
    return sceSifDmaStat(-6, local);
}
