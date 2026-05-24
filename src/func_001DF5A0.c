// CFLAGS: -O4,p -sdatathreshold 4
extern char *D_00275670;
extern char D_007635C0[8];
extern int func_001DF180(int);
extern void func_001CB760(char *, int, int, char *);

void func_001DF5A0(void) {
    char *t1;
    char *a3;
    char *v0;
    int a2;
    int a1;
    int t0;

    a2 = func_001DF180(3);
    t1 = D_00275670;
    t0 = 0x60;
    a3 = *(char **)(t1 + 0x1c);
    a1 = 0x00fff000;
    a3[3] = t0;
    v0 = *(char **)(t1 + 0x1c);
    *(int *)(v0 + 4) = 0;
    v0 = *(char **)(t1 + 0x1c);
    *(short *)v0 = 0;
    v0 = *(char **)(t1 + 0x1c);
    v0 = v0 + 0x10;
    func_001CB760(D_007635C0, a1, a2, a3);
    *(char **)(t1 + 0x1c) = v0;
}
