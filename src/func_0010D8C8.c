// COMPILER: eegcc
// CFLAGS: -O2
// SDK printf-style: temporarily install func_0010CF20 as D_00241C74 hook around func_0010D2C8.
extern void *D_00241C74;
extern void func_0010CF20(void);
extern void func_0010D2C8(int a0, void *args);

void func_0010D8C8(int a0, ...) {
    void *old = D_00241C74;
    void *ap = (char *)__builtin_next_arg(a0) - 56;
    D_00241C74 = (void *)func_0010CF20;
    func_0010D2C8(a0, ap);
    D_00241C74 = old;
}
