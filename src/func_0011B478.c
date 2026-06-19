// COMPILER: eegcc
// CFLAGS: -O2
// SDK leaf (ee-gcc 2.96): return func_0011B3F8() & 1.
extern int func_0011B3F8(void);

int func_0011B478(void) {
    return func_0011B3F8() & 1;
}
