extern volatile short D_00275B88; /* counter */
extern int *D_00275B80;  /* stack pointer */

void func_001B1D20(int *a0) {
    short v1;
    int a1;
    v1 = D_00275B88;
    if (v1 < 0x80) {
        a1 = a0[5];
        a0 = D_00275B80;
        D_00275B80 = (int *)((char *)a0 - 4);
        ((int *)((char *)a0 - 4))[0] = a1;
        v1 = D_00275B88;
        D_00275B88 = v1 + 1;
    }
}
