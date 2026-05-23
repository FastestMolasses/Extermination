extern volatile short D_00275BB8; /* counter */
extern int *D_00275BB0;  /* stack pointer */

void func_001B1C60(int *a0) {
    short v1;
    int a1;
    v1 = D_00275BB8;
    if (v1 < 0xC) {
        a1 = a0[5];
        a0 = D_00275BB0;
        D_00275BB0 = (int *)((char *)a0 - 4);
        ((int *)((char *)a0 - 4))[0] = a1;
        v1 = D_00275BB8;
        D_00275BB8 = v1 + 1;
    }
}
