extern volatile short D_00275B98; /* counter */
extern int *D_00275B90;  /* stack pointer */

void func_001B1CA0(int *a0) {
    short v1;
    int a1;
    v1 = D_00275B98;
    if (v1 < 0x40) {
        a1 = a0[5];
        a0 = D_00275B90;
        D_00275B90 = (int *)((char *)a0 - 4);
        ((int *)((char *)a0 - 4))[0] = a1;
        v1 = D_00275B98;
        D_00275B98 = v1 + 1;
    }
}
