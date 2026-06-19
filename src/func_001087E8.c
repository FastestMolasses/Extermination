// COMPILER: eegcc
// CFLAGS: -O2
// SDK leaf: advance ring offset a0->0x8 by (a1>>3), wrapping by a0->0x28 when >= a0->0x24.
unsigned int func_001087E8(unsigned int *a0, int a1) {
    unsigned int v = a0[2] + (a1 >> 3);
    if (v >= a0[9]) {
        v -= a0[10];
    }
    return v;
}
