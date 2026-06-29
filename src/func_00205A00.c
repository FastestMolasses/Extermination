// CFLAGS: -O4,p -sdatathreshold 4
//
// Bit-packing leaf: packs an 8-argument set (one 64-bit value + six ints) into
// a 4-word (0x10-byte) record at dst and returns the pointer just past it.
//   dst[0] = (f<<15)|g
//   dst[1] = (b<<28)|((c<<26)|((e<<14)|(d<<15)))   [OR association matches target]
//   dst[2] = low 32 bits of a
//   dst[3] = high 32 bits of a (dsrl32/dsll32/dsra32 extract; a is unsigned ll)
// Args 5-8 arrive in t0-t3 under the mwcc EE register-arg ABI. Both 991202 and
// 2.3.3 byte-match; 991202 preferred.
int *func_00205A00(int *dst, unsigned long long a, int b, int c, int d, int e, int f, int g) {
    dst[0] = (f << 15) | g;
    dst[1] = (b << 28) | ((c << 26) | ((e << 14) | (d << 15)));
    dst[2] = (int)a;
    dst[3] = (int)(a >> 32);
    return dst + 4;
}
