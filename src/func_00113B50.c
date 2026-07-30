// COMPILER: eegcc
// CFLAGS: -O2
extern int func_00112E28(int a0);
extern void func_0010DFD8(int *a0, int a1);
extern int func_0010E8A8(void *a, int b, int c, void *d, int e, void *f, int g, int h, int i);
extern void CreateSema(int a0);
extern int D_0027B090;
extern int D_0027AB40[];
extern int D_0027AF60[];
extern volatile int D_00241D0C;

int func_00113B50(int a0, int *a1) {
    int v;
    if (func_00112E28(0x1A) == 0) {
        return -1;
    }
    D_0027B090 = a0;
    func_0010DFD8(&D_0027B090, 4);
    if (func_0010E8A8(D_0027AF60, 5, 0, &D_0027B090, 4, D_0027AB40, 8, 0, 0) < 0) {
        CreateSema(D_00241D0C);
        return -1;
    }
    if (a1 != 0) {
        *a1 = *(int *)((int)&D_0027AB40[1] | 0x20000000);
    }
    v = *(int *)((int)&D_0027AB40[0] | 0x20000000);
    CreateSema(D_00241D0C);
    return v;
}
