// COMPILER: eegcc
// CFLAGS: -O2
// SDK leaf: store a0->0x14 into the (a0->0x10)-th word of array a1->0x1C.
void func_0010D928(int *a0, int *a1) {
    int *base = (int *)a1[7];
    base[a0[4]] = a0[5];
}
