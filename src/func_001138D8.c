// COMPILER: eegcc
// CFLAGS: -O2
extern int func_00112E28(int a0);
extern void func_0010DFD8(void *a0, int a1);
extern int func_0010E8A8(void *a0, int a1, int a2, void *a3, int a4, void *a5,
                         int a6, int a7, int a8);
extern int CreateSema(int a);
extern int D_0027B070;
extern int D_0027AB40;
extern int D_0027AF60;
extern int D_00241D0C;

int func_001138D8(int a0) {
    if (func_00112E28(0x22) == 0) {
        return 0;
    }
    D_0027B070 = a0;
    func_0010DFD8(&D_0027B070, 4);
    if (func_0010E8A8(&D_0027AF60, 0x22, 0, &D_0027B070, 4, &D_0027AB40, 4, 0, 0) < 0) {
        CreateSema(D_00241D0C);
        return 0;
    }
    {
        int r = *(int *)((int)&D_0027AB40 | 0x20000000);
        CreateSema(D_00241D0C);
        return r;
    }
}
