// COMPILER: eegcc
// CFLAGS: -O2
// SDK printf-style forwarder: stash D_0024295C in its sub-context, dispatch varargs.
extern int *D_0024295C;
extern void func_001236D8(int a0, int a1, void *args);

void func_00122B58(int a0, ...) {
    int *ctx = D_0024295C;
    void *ap = (char *)__builtin_next_arg(a0) - 56;
    int *sub = (int *)ctx[2];
    sub[0x54 / 4] = (int)ctx;
    func_001236D8(ctx[2], a0, ap);
}
