// COMPILER: eegcc
// CFLAGS: -O2
int func_00121E30(unsigned int a0) {
    int a1 = 0;
    if ((a0 & 0xFFFF0000) == 0) {
        a1 = 0x10;
        a0 <<= 16;
    }
    if ((a0 & 0xFF000000) == 0) {
        a1 += 8;
        a0 <<= 8;
    }
    if ((a0 & 0xF0000000) == 0) {
        a1 += 4;
        a0 <<= 4;
    }
    if ((a0 & 0xC0000000) == 0) {
        a1 += 2;
        a0 <<= 2;
    }
    if ((int)a0 >= 0) {
        a1 += 1;
        if ((a0 & 0x40000000) == 0) {
            return 0x20;
        }
    }
    return a1;
}
