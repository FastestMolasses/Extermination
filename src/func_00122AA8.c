// COMPILER: eegcc
// CFLAGS: -O2
extern long D_0026C818[];
extern long func_00126F88(long a0, long a1);

long func_00122AA8(int a0) {
    int s0 = a0;
    long v0 = (long)0xFFC0 << 46;
    if (s0 < 0x18) {
        return D_0026C818[s0];
    }
    if (s0 > 0) {
        do {
            v0 = func_00126F88(v0, (long)0x8048 << 47);
            s0--;
        } while (s0 > 0);
    }
    return v0;
}
