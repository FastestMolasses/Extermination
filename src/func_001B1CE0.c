extern volatile short D_00275B58; /* counter */
extern int *D_00275B54;  /* stack pointer */

void func_001B1CE0(int *a0) {
    short v1;
    int a1;
    v1 = D_00275B58;
    if (v1 < 0x40) {
        a1 = a0[5];
        a0 = D_00275B54;
        D_00275B54 = (int *)((char *)a0 - 4);
        ((int *)((char *)a0 - 4))[0] = a1;
        v1 = D_00275B58;
        D_00275B58 = v1 + 1;
    }
}
