// CFLAGS: -O4,p -sdatathreshold 0
extern int *D_00241CD8;
extern char D_0026BAD0[8];
extern void func_00123168(int, int);

void func_00110C48(int a0, int a1) {
    int v0;
    int v1;
    if (a0 < 4) {
        v0 = (int)D_00241CD8 + a0 * 4;
        func_00123168(a1, *(int *)v0);
        return;
    }
    v1 = *(unsigned char *)D_0026BAD0;
    *(char *)a1 = v1;
}
