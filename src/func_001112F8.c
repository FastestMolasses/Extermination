// COMPILER: eegcc
// CFLAGS: -O2
// SDK leaf (ee-gcc 2.96): call func_001111F8(a, b, 0).
extern int func_001111F8(int a, int b, int c);

int func_001112F8(int a, int b) {
    return func_001111F8(a, b, 0);
}
