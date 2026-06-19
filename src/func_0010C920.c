// COMPILER: eegcc
// CFLAGS: -O2
// SDK wrapper: query SIF DMA status into a 2-word local for channel -7.
extern int sceSifDmaStat(int a0, void *a1);

int func_0010C920(int a0, int a1) {
    int local[2];
    local[0] = a0;
    local[1] = (signed char)a1;
    return sceSifDmaStat(-7, local);
}
