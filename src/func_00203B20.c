// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Movie frame ring: r+0 = a0, r+4 = the slot buffer, r+0x10 = the slot
// count; both cursors (+8 / +0xC) start at 0 and the first word of every
// 0x39640-byte slot is cleared.
void func_00203B20(int *r, int a, char *slots, int n) {
    int i;

    r[0] = a;
    *(char **)(r + 1) = slots;
    r[4] = n;
    r[3] = 0;
    r[2] = 0;
    for (i = 0; i < n; i++) {
        *(int *)(*(char **)(r + 1) + i * 0x39640) = 0;
    }
}
