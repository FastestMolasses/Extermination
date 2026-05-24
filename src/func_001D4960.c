// CFLAGS: -O4,p -sdatathreshold 4
extern char *D_00275670;
extern char D_00816440[0x10000];
extern void func_001D4750(int);
extern void func_001D2090(int, char *);
extern char D_00239C90[8];

void func_001D4960(void) {
    char *t0;
    char *v1;
    char *a2;
    int a3;
    int a1;
    int a0;

    func_001D4750(0);
    func_001D2090(0, D_00239C90);

    t0 = D_00275670;
    a2 = D_00816440;
    a1 = 0x30;
    a3 = *(int *)(t0 + 0x9c);
    v1 = *(char **)(t0 + 0x10);
    a0 = 8;
    a3 = a3 << 7;
    v1[3] = a1;
    v1 = *(char **)(t0 + 0x10);
    a1 = (int)(a2 + a3);
    *(int *)(v1 + 4) = a1;
    v1 = *(char **)(t0 + 0x10);
    *(short *)v1 = a0;
    v1 = *(char **)(t0 + 0x10);
    v1 = v1 + 0x10;
    *(char **)(t0 + 0x10) = v1;
}
