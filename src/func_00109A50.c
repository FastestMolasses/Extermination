// COMPILER: eegcc
// CFLAGS: -O2
// SDK leaf (ee-gcc 2.96): reset a render context block, then re-init.
extern void func_0010A298(void);
extern void func_0010A1B0(void);
extern int D_00241404;

void func_00109A50(int *ctx) {
    int *p = (int *)ctx[0x40 / 4];
    p[0] = 0;
    p[1] = 0;
    p[2] = 0;
    ctx[0x8 / 4] = 0;
    p[0x80 / 4] = -1;
    p[0xAC / 4] = 0;
    func_0010A298();
    D_00241404 = 0;
    func_0010A1B0();
}
