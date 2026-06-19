// COMPILER: eegcc
// CFLAGS: -O2
// SDK leaf: copy three uncached MMIO words into optional global pointers.
extern int *D_0027B0E8;
extern int *D_0027B0EC;
extern int *D_0027B0F0;

void func_00114930(int a0) {
    int *p = (int *)(a0 | 0x20000000);
    if (D_0027B0E8) {
        *D_0027B0E8 = p[0];
    }
    if (D_0027B0EC) {
        *D_0027B0EC = p[1];
    }
    if (D_0027B0F0) {
        *D_0027B0F0 = p[0x24];
    }
}
