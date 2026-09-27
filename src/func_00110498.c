// COMPILER: eegcc
// CFLAGS: -O2
// SDK wrapper: return (sceSifGetReg(4) & 0x10000) != 0.
extern int sceSifGetReg(int a0);

int func_00110498(void) {
    int v = sceSifGetReg(4) & 0x10000;
    return v != 0;
}
