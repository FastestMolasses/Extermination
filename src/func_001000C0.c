// CFLAGS: -O4,p -sdatathreshold 0
// Returns whether the called function's result is negative.
extern int func_001274B0(void);

int func_001000C0(void) {
    return func_001274B0() < 0;
}
