// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
extern int D_0026EC60;
extern unsigned char D_008106F4;
extern unsigned char D_00810700;
extern void func_001FA790(int, int);
extern void func_001FD470(int, int *, int *, unsigned char);

int func_001FD4C0(int arg0) {
    int s0;
    int *base;
    int *a1;
    int *a2;

    s0 = 0;
    base = &D_0026EC60;
    a1 = &D_0026EC60;
    a2 = &D_0026EC60;
    while (*a2 != -1) {
        if (D_00810700 == a1[0] && arg0 == a1[2]) {
            func_001FD470(-1, a1, a2, D_00810700);
            D_008106F4 = 2;
            func_001FA790(0, base[s0 * 4 + 3]);
            return 1;
        }
        a1 = (int *)((char *)a1 + 0x10);
        a2 = (int *)((char *)a2 + 0x10);
        s0 += 1;
    }
    return 0;
}
