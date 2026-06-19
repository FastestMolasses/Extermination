// COMPILER: eegcc
// CFLAGS: -O2
extern int func_0010C360(int);
extern int AddDmacHandler(int, int);
extern int D_00277214;
extern int D_00241C78;
void func_0010DC40(void) {
    int r = func_0010C360(5);
    r = AddDmacHandler(5, D_00277214);
    D_00241C78 = 0;
    (void)r;
}
