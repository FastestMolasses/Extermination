// COMPILER: eegcc
// CFLAGS: -O2
extern int D_0027AB40[];
extern char D_0027AF60[];
extern volatile int D_00241D0C;
extern int func_00112E28(int a);
extern int func_0010E8A8(void *a, int b, int c, int d, int e, void *f, int g, int h, int i);
extern void CreateSema(int a);

int func_00113680(void) {
    int *s0;
    if (func_00112E28(3) == 0) {
        return -1;
    }
    s0 = D_0027AB40;
    if (func_0010E8A8(D_0027AF60, 4, 0, 0, 0, s0, 4, 0, 0) < 0) {
        CreateSema(D_00241D0C);
        return -1;
    }
    CreateSema(D_00241D0C);
    return *(int *)((int)s0 | 0x20000000);
}
