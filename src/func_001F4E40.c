// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Draws a pulsing glow sprite at pos. t = ((scratchpad word 0x70003B68 *
// rate) & 0x1FF) - 0x100, made positive; the brightness a = col[3] * t / 256 is
// raised by a random byte (bits 23..30 of rand) as (a + a * k / 256) / 2, and
// r, g, b = col[0..2] * a / 128 are packed 0xBBGGRR. func_001CD520(0, 2, pos,
// 0x20045B0599421EF0, size, size, 1.5, colour) draws it.
extern int D_70003B68;
extern int func_00122BB8(void);
extern void func_001CD520(int kind, int mode, void *pos, long long tag, float w, float h, float z, unsigned int rgb);

void func_001F4E40(void *pos, unsigned int *col, int rate, float size) {
    unsigned int a = col[3];
    int t = ((D_70003B68 * rate) & 0x1FF) - 0x100;
    unsigned int c;

    if (t < 0) {
        t = -t;
    }
    a *= t;
    a >>= 8;
    a += (a * ((func_00122BB8() >> 23) & 0xFF)) >> 8;
    a >>= 1;
    c = ((col[2] * a) >> 7) << 16;
    c |= ((col[1] * a) >> 7) << 8;
    c |= (col[0] * a) >> 7;
    func_001CD520(0, 2, pos, 0x20045B0599421EF0LL, size, size, 1.5f, c);
}
