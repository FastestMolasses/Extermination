// CFLAGS: -O4,p -sdatathreshold 0
extern int D_00241170[];
extern int D_00241324;

void func_001063B8(unsigned int a0) {
    volatile int *hw = (int *)0x10002000;
    *hw = a0;
    D_00241324 = D_00241170[a0 >> 28];
}
