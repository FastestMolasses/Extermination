// COMPILER: eegcc
// CFLAGS: -O2

extern int func_00111F18(int);
extern int func_00122B58(const char *, ...);
extern void func_0010DFD8(void *, int);
extern int func_0010E8A8(void *, int, int, void *, int, void *, int, void *, void *);
extern int CreateSema(int);

extern int D_00241CF8;
extern int D_00241D08;
extern unsigned char D_0027B0A0[];
extern unsigned char D_00279B80[];
extern unsigned char D_00279E00[];
extern char D_0026BEA8[];
extern char D_0026BEC0[];
extern char D_0026BC78[];

int func_00113F68(int a0, int a1, int a2, int a3, unsigned char *a4)
{
    unsigned char *p;
    int ret;

    if (func_00111F18(0xF) == 0)
        return 0;

    if (D_00241CF8 > 0)
        func_00122B58(D_0026BEA8);

    p = D_0027B0A0;
    *(int *)(p + 0x0) = a0;
    *(int *)(p + 0x4) = a1;
    *(int *)(p + 0x8) = a2;
    *(int *)(p + 0xC) = a3;
    if (a4 != 0) {
        p[0x10] = a4[0];
        p[0x11] = a4[1];
        p[0x12] = a4[2];
    }

    if (D_00241CF8 > 0)
        func_00122B58(D_0026BEC0);

    func_0010DFD8(p, 0x14);

    if (func_0010E8A8(D_00279E00, 9, 0, p, 0x14, D_00279B80, 4, 0, 0) < 0) {
        CreateSema(D_00241D08);
        return 0;
    }

    if (D_00241CF8 > 0)
        func_00122B58(D_0026BC78);

    ret = *(int *)((unsigned int)D_00279B80 | 0x20000000);
    CreateSema(D_00241D08);
    return ret;
}
