// COMPILER: eegcc
// CFLAGS: -O2
// SDK wrapper: return func_00110048(a0, a1, a2, 1).
extern int func_00110048(int a0, int a1, int a2, int a3);

int func_00110150(int a0, int a1, int a2) {
    return func_00110048(a0, a1, a2, 1);
}
