extern volatile short D_00275B68; /* counter */
extern int *D_00275B60;  /* stack pointer */

void func_001B1DE0(int *a0) {
    short v1;
    int a1;
    v1 = D_00275B68;
    if (v1 < 0x20) {
        a1 = a0[5];
        a0 = D_00275B60;
        D_00275B60 = (int *)((char *)a0 - 4);
        ((int *)((char *)a0 - 4))[0] = a1;
        v1 = D_00275B68;
        D_00275B68 = v1 + 1;
    }
}
