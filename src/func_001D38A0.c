// CFLAGS: -O4,p -sdatathreshold 4
extern char *D_00275670;
extern char D_00816440[0x10000];
extern void func_001D37D0(void);

void func_001D38A0(int a0) {
    char *v1 = D_00275670;
    char *t0 = D_00816440;
    int v0 = a0 << 2;
    int t1 = *(int *)(v1 + 0x9c);
    char **a3 = (char **)(v1 + v0);
    char *ptr = a3[4];
    t1 = t1 << 7;
    t0 = t0 + t1;
    ptr[3] = 0x30;
    ptr = a3[4];
    *(char **)(ptr + 4) = t0;
    ptr = a3[4];
    *(short *)ptr = 8;
    ptr = a3[4];
    a3[4] = ptr + 0x10;
    func_001D37D0();
}
