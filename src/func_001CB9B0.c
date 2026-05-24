// CFLAGS: -O4,p -sdatathreshold 4
extern int D_00275674;

int func_001CB9B0(int a0) {
    if (a0 == 4) goto L_4;
    if (a0 == 3) goto L_3;
    if (a0 == 2) goto L_2;
    if (a0 == 1) goto L_1;
    if (a0 == 0) goto L_0;
    goto L_ret;
L_0:
    return D_00275674 + 0x6A0;
L_1:
    return D_00275674 + 0x720;
L_2:
    return D_00275674 + 0x7A0;
L_3:
    return D_00275674 + 0x820;
L_4:
    return D_00275674 + 0x8A0;
L_ret:;
}
