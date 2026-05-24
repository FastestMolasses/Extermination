// CFLAGS: -O4,p -sdatathreshold 0
extern int *D_0024295C;

int func_00122BB8(void) {
    int v0;
    int *a1;
    int v1;
    int a0;
    v0 = 0;
    a1 = D_0024295C;
    a0 = 0x41c64e6d;
    v0 = 0x7fffffff;
    v1 = a1[0x16];
    v1 = v1 * a0 + 0x3039;
    a1[0x16] = v1;
    return v1 & v0;
}
