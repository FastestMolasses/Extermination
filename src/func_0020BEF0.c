// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Wrap/clamp helper: sum two bytes (offsets 0x17, 0x19); if the sum reaches
// the modulus byte (offset 0x18) subtract it once. Returns the wrapped value.
int func_0020BEF0(unsigned char *a0) {
    int v0;

    v0 = a0[0x17] + a0[0x19];
    if (v0 >= a0[0x18]) {
        v0 -= a0[0x18];
    }
    return v0;
}
