// CFLAGS: -O4,p -sdatathreshold 0
extern int D_00277224;
extern int D_0027722C;

void func_0010DCD8(int a0) {
    int *p;
    p = (int *)(a0 * 8 + (a0 < 0 ? D_00277224 : D_0027722C));
    *p = 0;
}
