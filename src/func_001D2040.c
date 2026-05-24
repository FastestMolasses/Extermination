// CFLAGS: -O4,p -sdatathreshold 4
extern char *D_00275674;
extern char **D_00275670;

void func_001D2040(int a0, int a1) {
    int a2 = a1 << 6;
    char *a1b;
    char **a3;
    int a4 = a0;
    char *ptr;
    a1b = D_00275674;
    a3 = D_00275670;
    a1b = a1b + a2;
    a3 = (char **)((char *)a3 + (a4 << 2));
    a1b = a1b + 0x5a0;
    ptr = a3[4];
    ptr[3] = 0x30;
    ptr = a3[4];
    *(char **)(ptr + 4) = a1b;
    ptr = a3[4];
    *(short *)ptr = 4;
    ptr = a3[4];
    a3[4] = ptr + 0x10;
}
