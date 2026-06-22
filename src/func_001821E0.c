// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// If the two floats at +0x224/+0x22C are both 0 and flag bit 2 at +0xF is
// clear, do nothing and return 0. Otherwise set bytes +4=2, +5=9, +6=0 and
// return 1.
int func_001821E0(void *a) {
    float zero = 0.0f;
    if (*(float *)((char *)a + 0x224) != zero ||
        *(float *)((char *)a + 0x22C) != zero ||
        (*(unsigned char *)((char *)a + 0xF) & 2)) {
        *(char *)((char *)a + 4) = 2;
        *(char *)((char *)a + 5) = 9;
        *(char *)((char *)a + 6) = 0;
        return 1;
    }
    return 0;
}
