// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Leaf: byte-checksum over a 0x640-byte block.
// Sums bytes [0..0x63E] with an 8-bit wrapping accumulator, sets a
// validity flag byte at +0xF to 1, and writes the 8-bit two's-complement
// of the sum at +0x63F so the whole region sums to zero.
void func_002284F0(unsigned char *p) {
    unsigned int i;
    int sum;

    sum = 0;
    for (i = 0; i < 0x63F; i++) {
        sum = (sum + p[i]) & 0xFF;
    }
    p[0xF] = 1;
    p[0x63F] = 0x100 - sum;
}
