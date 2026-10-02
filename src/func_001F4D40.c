// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Draws a flickering glow sprite at pos. col points at four unsigned words
// (r, g, b, a); one random byte k (bits 23..30 of func_00122BB8()) gives the
// brightness s = (3a + a * k / 256) / 4, and each of r, g, b is scaled by
// s / 128 into a packed 0xBBGGRR colour. func_001CD520(0, 2, pos,
// 0x20045B0599421EF0, size, size, depth, colour) draws it.
extern int func_00122BB8(void);
extern void func_001CD520(int kind, int mode, void *pos, long long tag, float w, float h, float z, unsigned int rgb);

void func_001F4D40(void *pos, unsigned int *col, float size, float depth) {
    unsigned int a = col[3];
    unsigned int s;
    unsigned int c;

    s = a * 3 + ((a * ((func_00122BB8() >> 23) & 0xFF)) >> 8);
    s >>= 2;
    c = ((col[2] * s) >> 7) << 16;
    c |= ((col[1] * s) >> 7) << 8;
    c |= (col[0] * s) >> 7;
    func_001CD520(0, 2, pos, 0x20045B0599421EF0LL, size, size, depth, c);
}
