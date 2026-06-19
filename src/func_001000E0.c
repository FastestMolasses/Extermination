// CFLAGS: -O4,p -sdatathreshold 0
// Returns whether the callee's result is non-positive.
extern int func_001274B0(void);

int func_001000E0(void) {
    return func_001274B0() <= 0;
}
