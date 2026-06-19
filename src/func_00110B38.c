// COMPILER: eegcc
// CFLAGS: -O2
extern int *func_00110AB8(int a0, int a1, int a2);
extern void block_copy(int a0, int *a1, int a2);

int func_00110B38(int a0, int a1, int a2) {
    int *s0 = func_00110AB8(a0, a1, a2);
    block_copy(a2, s0, s0[0x60 / 4]);
    return s0[0x60 / 4];
}
