// COMPILER: eegcc
// CFLAGS: -O2
// Normalize a bitmask in *a0, returning a code or trailing-zero count.
int func_00121EB8(int *a0) {
    unsigned int v = *a0;
    int n;
    if (v & 7) {
        if (v & 1)
            return 0;
        if (v & 2) {
            *a0 = v >> 1;
            return 1;
        }
        *a0 = v >> 2;
        return 2;
    }
    n = 0;
    if ((v & 0xFFFF) == 0) {
        n = 0x10;
        v >>= 16;
    }
    if ((v & 0xFF) == 0) {
        n += 8;
        v >>= 8;
    }
    if ((v & 0xF) == 0) {
        n += 4;
        v >>= 4;
    }
    if ((v & 3) == 0) {
        n += 2;
        v >>= 2;
    }
    if ((v & 1) == 0) {
        v >>= 1;
        if (v == 0)
            return 0x20;
        n += 1;
    }
    *a0 = v;
    return n;
}
