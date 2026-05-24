// CFLAGS: -O4,p -sdatathreshold 0
extern char D_0028A750;
extern char D_0028A754;

void func_001AB740(int a0, int a1) {
    long v1 = a0;
    int a2 = (int)(v1 << 5);
    char *a3 = &D_0028A750 + a2;
    char *a3b = &D_0028A754 + a2;
    *(char *)a3 = 1;
    *(int *)a3b = a1;
    *(int *)(a3 + 0x8) = 0;
    *(int *)(a3 + 0xc) = 0;
    *(int *)(a3 + 0x10) = 0;
    *(int *)(a3 + 0x14) = 0;
}
