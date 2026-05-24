// CFLAGS: -O4,p -sdatathreshold 4
extern signed char D_00275B30;
extern int D_00281CF0[0x40];

int func_001FA5A0(int a0) {
    int a2 = D_00275B30;
    int *v1 = D_00281CF0;
    int *a1 = v1 + a2;
    int v0 = -1;
    int v1v = *a1;
    if (v1v != v0) {
        return 1;
    }
    v0 = a2 + 1;
    *a1 = a0;
    v0 = v0 & 0xf;
    D_00275B30 = v0;
    return 1;
}
