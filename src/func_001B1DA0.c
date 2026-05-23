extern volatile short D_00275BA8; /* counter */
extern int *D_00275BA0;  /* stack pointer */

void func_001B1DA0(int *a0) {
    short v1;
    int a1;
    v1 = D_00275BA8;
    if (v1 < 0x30) {
        a1 = a0[5];
        a0 = D_00275BA0;
        D_00275BA0 = (int *)((char *)a0 - 4);
        ((int *)((char *)a0 - 4))[0] = a1;
        v1 = D_00275BA8;
        D_00275BA8 = v1 + 1;
    }
}
