// COMPILER: eegcc
// CFLAGS: -O2
// SDK predicate (ee-gcc 2.96): returns whether func_00111188 returned 0x3FFFF.
extern int func_00111188(void);

int func_001112A8(void) {
    return func_00111188() == 0x3FFFF;
}
