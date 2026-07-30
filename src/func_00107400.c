// COMPILER: eegcc
// CFLAGS: -O2
extern int func_00106948(int a);

extern int D_00241504;
extern int D_00241508;
extern int D_0024150C;
extern int D_00241510;
extern int D_00241514;
extern int D_00241518;

void func_00107400(void) {
    int *p;

    p = &D_00241504;
    *p = func_00106948(1);
    p = &D_00241508;
    *p = func_00106948(8);
    p = &D_0024150C;
    *p = func_00106948(1);
    func_00106948(7);
    func_00106948(1);
    p = &D_00241510;
    *p = func_00106948(0x14);
    func_00106948(1);
    p = &D_00241514;
    *p = func_00106948(0x16);
    func_00106948(1);
    p = &D_00241518;
    *p = func_00106948(0x16);
}
