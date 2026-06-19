// COMPILER: eegcc
// CFLAGS: -O2
// SDK wrapper: return (RFU116(4) & 0x10000) != 0.
extern int RFU116(int a0);

int func_00110498(void) {
    int v = RFU116(4) & 0x10000;
    return v != 0;
}
