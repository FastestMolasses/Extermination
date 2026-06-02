// Returns whether func_001BA1F0 reported a non-zero status. The ternary
// (rather than a bare `!= 0`) steers mwcc to the movz idiom the original used.
extern int func_001BA1F0(void);

int func_001BB7C0(void) {
    int status = func_001BA1F0();
    return status != 0 ? 1 : 0;
}
