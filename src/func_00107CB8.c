// CFLAGS: -O4,p -sdatathreshold 0
extern int *D_002412F4;
extern int D_00241404;
extern int D_00241B80;

int func_00107CB8(void) {
    int v0;
    int *v1;
    int *a0;
    int a1;
    v0 = *(int *)(int)D_002412F4;
    a1 = 2;
    v1 = (int *)*(int *)v0;
    a0 = (int *)v1[16];
    v0 = a0[2];
    if (v0 != a1) {
        v1 = (int *)D_00241404;
        a0[2] = a1;
        a0[0x2b] = *v1;
    }
    v1 = (int *)&D_00241B80;
    v0 = 1;
    *v1 = v0;
    return v0;
}
