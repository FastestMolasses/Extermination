// CFLAGS: -O4,p -sdatathreshold 4
extern char *D_00275670;
extern char *D_00275674;

void func_001D1F20(int a0) {
    char *t0;
    int v1;
    char *a3;
    char **a2;
    int t1;
    char *ptr;
    int t0b;
    int a1;
    t0 = D_00275670;
    v1 = a0 << 2;
    a3 = D_00275674;
    t1 = *(int *)(t0 + 0x9c);
    a2 = (char **)(t0 + v1);
    ptr = a2[4];
    t0b = t1 * 4;
    t0b = t0b + t1;
    ptr[3] = 0x30;
    a1 = t0b * 4;
    a1 = t0b + a1;
    ptr = a2[4];
    a1 = a1 << 4;
    a1 = (int)(a3 + a1) + 0x20;
    *(int *)(ptr + 4) = a1;
    ptr = a2[4];
    *(short *)ptr = 0x19;
    ptr = a2[4];
    a2[4] = ptr + 0x10;
}
