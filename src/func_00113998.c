// COMPILER: eegcc
// CFLAGS: -O2
extern int func_00112E28(int a0);
extern void func_0010DFD8(int *a0, int a1);
extern int func_0010E8A8(void *a, int b, int c, void *d, int e, void *f, int g, int h, int i);
extern void CreateSema(int a0);
extern int D_0027B080;
extern int D_0027AB40[];
extern int D_0027AF60[];
extern int D_00241D0C;

int func_00113998(int a0) {
    int v;
    if (func_00112E28(0x23) == 0) {
        return 0;
    }
    D_0027B080 = a0;
    func_0010DFD8(&D_0027B080, 4);
    if (func_0010E8A8(D_0027AF60, 0x23, 0, &D_0027B080, 4, D_0027AB40, 4, 0, 0) < 0) {
        CreateSema(D_00241D0C);
        return 0;
    }
    v = *(int *)((int)&D_0027AB40[0] | 0x20000000);
    CreateSema(D_00241D0C);
    return v;
}
