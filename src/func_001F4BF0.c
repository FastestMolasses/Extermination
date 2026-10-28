// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a readable companion of an
// asm body at 99.70%). Fix read from the original instructions:
// - The shared 3.0 sprite size is materialised in the second float-argument register and
//   copied down to the first. Spelling the second size as an int 3 converted to float
//   (idiom-24, as in func_00135D00's restore calls) binds the constant there; a plain
//   `3.0f, 3.0f` binds it to the first register and copies up.
//
// Draws a flickering glow sprite at pos. col points at four unsigned words (r, g, b, a);
// one random byte k (bits 23..30 of func_00122BB8()) raises the brightness as
// a = (a + a * k / 256) / 2, and each of r, g, b is scaled by a / 128 into a packed
// 0xBBGGRR colour. func_001CD520(0, 2, pos, 0x20045B0599421EF0, colour, 3.0, 3.0, 1.5)
// draws it (same family as func_001F4D40 / func_001F4E40).
extern int func_00122BB8(void);
extern int func_001CD520(int bucket, int mode, void *world, unsigned long long giftag,
                         unsigned int rgba, float w, float h, float zbias);

void func_001F4BF0(void *pos, unsigned int *col) {
    unsigned int a = col[3];
    unsigned int c;

    a += (a * ((func_00122BB8() >> 23) & 0xFF)) >> 8;
    a >>= 1;
    c = ((col[2] * a) >> 7) << 16;
    c |= ((col[1] * a) >> 7) << 8;
    c |= (col[0] * a) >> 7;
    {
        int ti = 3;
        float t = (float)ti;
        func_001CD520(0, 2, pos, 0x20045B0599421EF0LL, c, 3.0f, t, 1.5f);
    }
}
