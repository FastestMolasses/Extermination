// COMPILER: eegcc
// CFLAGS: -O2
// HW status decode at 0x10003800: classify by bit groups.
int func_0011B6E0(void) {
    int x = *(volatile int *)0x10003800;
    if ((x & 3) == 0) {
        return 0;
    }
    if (x & 0x700) {
        return 1;
    }
    if (x & 0xC) {
        return 2;
    }
    return (x & 0x3000) ? 3 : 4;
}
