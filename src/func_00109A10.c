// COMPILER: eegcc
// CFLAGS: -O2
// SDK leaf: copy three consecutive fields of a0->0x40 into out pointers.
void func_00109A10(int *a0, int *a1, int *a2, int *a3) {
    int *p = (int *)a0[0x40 / 4];
    *a1 = p[0x94 / 4];
    *a2 = p[0x98 / 4];
    *a3 = p[0x9C / 4];
}
