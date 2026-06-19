// COMPILER: eegcc
// CFLAGS: -O2
// SDK leaf (ee-gcc 2.9): clear field 0x18, mask off bit0 of field 0x10.
void func_0010E318(int *a0) {
    a0[0x18 / 4] = 0;
    a0[0x10 / 4] &= 0xFFFFFFFE;
}
