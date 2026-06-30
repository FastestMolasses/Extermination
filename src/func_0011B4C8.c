// COMPILER: eegcc
// CFLAGS: -O2
extern int func_0011B498(int *p);

int func_0011B4C8(int *p) {
    if (func_0011B498(p) == 2) {
        return 0;
    }
    p[0] = *(volatile int *)0x10003040;
    p[1] = *(volatile int *)0x10003050;
    p[2] = *(volatile int *)0x10003060;
    p[3] = *(volatile int *)0x10003070;
    p[4] = *(volatile int *)0x10003020;
    p[5] = *(volatile int *)0x10003080;
    p[6] = *(volatile int *)0x10003090;
    p[7] = *(volatile int *)0x100030A0;
    return 1;
}
