// COMPILER: eegcc
// CFLAGS: -O2
// a0==0: spin until MMIO 0x10002010 >= 0. a0==1: result = top bit. else 0.
int func_0010B0F8(int a0) {
    int r = 0;
    switch (a0) {
    case 0:
        while (*(volatile int *)0x10002010 < 0) {
        }
        r = 0;
        break;
    case 1:
        r = (unsigned int)*(volatile int *)0x10002010 >> 31;
        break;
    }
    return r;
}
