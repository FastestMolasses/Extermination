// COMPILER: eegcc
// CFLAGS: -O2
// BCD time -> frame count: h=bcd(b0), m=bcd(b1), s=bcd(b2);
// return ((h*60 + m)*75 + s) - 150.
int func_00111A80(unsigned char *a0) {
    unsigned char b0 = a0[0];
    unsigned char b1 = a0[1];
    unsigned char b2 = a0[2];
    int h = (b0 >> 4) * 10 + (b0 & 0xF);
    int m = (b1 >> 4) * 10 + (b1 & 0xF);
    int s = (b2 >> 4) * 10 + (b2 & 0xF);
    return (h * 60 + m) * 75 + s - 150;
}
