// COMPILER: eegcc
// CFLAGS: -O2

extern int func_00112E28(int);
extern int func_0010E8A8(void *, int, int, int, int, void *, int, int, int);
extern int CreateSema(int);

extern int D_0027AB40;
extern char D_0027AF60;
extern volatile int D_00241D0C;

int func_00113530(void) {
    int *s0;
    int r;

    if (func_00112E28(1) == 0)
        return 0;
    s0 = &D_0027AB40;
    r = func_0010E8A8(&D_0027AF60, 3, 0, 0, 0, s0, 4, 0, 0);
    if (r < 0) {
        CreateSema(D_00241D0C);
        return 0;
    }
    CreateSema(D_00241D0C);
    return *(int *)((unsigned int)s0 | 0x20000000);
}
