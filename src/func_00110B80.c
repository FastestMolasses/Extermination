// COMPILER: eegcc
// CFLAGS: -O2
// SDK leaf (ee-gcc 2.9): map a status byte; 6+2 -> 5, else passthrough.
extern unsigned char *func_00110AB8(void);

int func_00110B80(void) {
    unsigned char *p = func_00110AB8();
    if (p[0x70] == 6 && p[0x71] == 2) {
        return 5;
    }
    return p[0x70];
}
