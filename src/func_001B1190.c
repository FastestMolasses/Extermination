// CFLAGS: -O4,p -sdatathreshold 0
extern unsigned char D_00810700;
extern int D_00810860[0x100];

void func_001B1190(int a0) {
    int v1 = a0 & 0xff;
    int a2, a1;
    int *a0p;
    if (v1 == 0) { return; }
    a2 = a0 & 0x1f;
    a1 = D_00810700;
    a2 = 1 << a2;
    a0p = D_00810860;
    a1 = a1 << 5;
    a0p = (int *)((char *)a0p + a1);
    v1 = v1 >> 5;
    v1 = v1 << 2;
    a0p = (int *)((char *)a0p + v1);
    *a0p = *a0p | a2;
}
