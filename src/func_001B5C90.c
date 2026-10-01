// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Raises a stick/colour byte by 2 and snaps it to a multiple of 4, saturating
// at 0xFC: (x + 2) & 0xFC while the sum stays below 0x100, else 0xFC.
int func_001B5C90(unsigned char x) {
    unsigned short v = x + 2;

    if (v >= 0x100) {
        return 0xFC;
    } else {
        return v & 0xFC;
    }
}
