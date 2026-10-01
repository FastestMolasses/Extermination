// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
int func_0021BC40(unsigned char *e) {
    unsigned char k = e[0x1F0];
    if (k == 0x0B || k == 0x0C || k == 0x0D || (k == 0x2C && e[0xD] == 2) || k == 0x10 || k == 0x17 || k == 0x1A || k == 0x1D || k == 0x2A || k == 0x21 || k == 0x22 || k == 0x2F || k == 0x30 || k == 0x39 || k == 0x2D) return 1;
    return 0;
}
