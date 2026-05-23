extern volatile short D_00275B78; /* counter */
extern int *D_00275B70;  /* stack pointer */

void func_001B1D60(int *a0) {
    short v1;
    int a1;
    v1 = D_00275B78;
    if (v1 < 0x40) {
        a1 = a0[5];
        a0 = D_00275B70;
        D_00275B70 = (int *)((char *)a0 - 4);
        ((int *)((char *)a0 - 4))[0] = a1;
        v1 = D_00275B78;
        D_00275B78 = v1 + 1;
    }
}
