// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Returns 0 or 1 for an object: 1 when self+0x236 is set or (bit 1 of self+0
// set and self+0x1F0 != 0x3B). With self+4 == 1: 0 for self+5 in {0, 1, 0x21,
// 0x22}, and for 0x1D or 0x1E when self+0x1F1 is 1; 1 otherwise. With self+4 ==
// 2: 0 when self+5 is 0x0B. 1 in every other case.
int func_0021BD60(unsigned char *p) {
    unsigned char s;
    if (p[0x236]) {
        return 1;
    }
    if ((p[0] & 2) && p[0x1F0] != 0x3B) {
        return 1;
    }
    s = p[4];
    if (s == 1) {
        s = p[5];
        if (s == 0 || s == 1 || s == 0x21 || s == 0x22 ||
            (s == 0x1D && p[0x1F1] == 1) || (s == 0x1E && p[0x1F1] == 1)) {
            return 0;
        }
    } else if (s == 2 && p[5] == 0xB) {
        return 0;
    }
    return 1;
}
