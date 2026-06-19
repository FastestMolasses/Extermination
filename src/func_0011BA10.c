// COMPILER: eegcc
// CFLAGS: -O2
int func_0011BA10(void) {
    int a0 = *(volatile int *)0x10003C00;
    if ((a0 & 0x3) == 0) return 0;
    if ((a0 & 0x700) != 0) return 1;
    if ((a0 & 0xC) != 0) return 2;
    return (a0 & 0x3000) != 0 ? 3 : 4;
}
