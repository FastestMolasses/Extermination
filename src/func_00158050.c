// CFLAGS: -O4,p -sdatathreshold 0
extern char D_700038A0[0x10];
extern char D_008105E0[0x100];
extern int func_00102948(char *, char *, int);

int func_00158050(int a0) {
    char *v0;
    char *a1;
    int a2;
    a0 = *(int *)((char *)a0 + 28);
    a1 = D_700038A0;
    a2 = 0x3f800000;
    v0 = *(char **)((char *)a0 + 28);
    a2 = 0x3f800000;
    *(float *)0x700038a0 = *(float *)(v0 + 0xb0);
    *(float *)0x700038a4 = *(float *)(v0 + 0xb4);
    *(float *)0x700038a8 = *(float *)(v0 + 0xb8);
    *(int *)0x700038ac = a2;
    func_00102948(D_008105E0, a1, a2);
    return 1;
}
