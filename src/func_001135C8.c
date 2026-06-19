// COMPILER: eegcc
// CFLAGS: -O2
extern int func_00112E28(int a0);
extern int func_0010E8A8(void *a, int b, int c, int d, int e, void *f, int g, int h, int i);
extern void CreateSema(int a0);
extern void func_00122B58(int *a0);
extern int D_0027AB40[];
extern int D_0027AF60[];
extern int D_00241D0C;
extern int D_00241CF8;
extern int D_0026BD78;

int func_001135C8(void) {
    int v;
    if (func_00112E28(2) == 0) {
        return -1;
    }
    if (func_0010E8A8(D_0027AF60, 0xC, 0, 0, 0, D_0027AB40, 4, 0, 0) < 0) {
        CreateSema(D_00241D0C);
        return -1;
    }
    v = *(int *)((int)&D_0027AB40[0] | 0x20000000);
    CreateSema(D_00241D0C);
    if (D_00241CF8 >= 2) {
        func_00122B58(&D_0026BD78);
    }
    return v;
}
