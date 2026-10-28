// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// Scrolling step of the 5-row list menu p (same menu record as
// func_0020B210; tbl = its 24-byte glyph records, g = cursor-row glyph).
// Clips drawing to the list window (func_00207D90), then draws the 5
// visible rows p[0x90..0x94] (two dim 0x40404040 quads at x 0x77F0 and
// 0x7FF0, glyphs +0 and +8 of the row's record) at y = 0x6A + 0x30*i plus
// the scroll offset p+0x1C (and -0x30 when scrolling down, p[0x1A] == 1).
// Moves the offset 4 units (down) or -4 (up); when it passes a full row
// (-0x30 / +0x30) the ring head p[0x19] steps with wrap-around over the
// item count p[0x18] and the function returns 1 (else 0). Finally draws
// the cursor row highlight (glyph g, 0x20808080) at row p[0x17].

extern int float_to_int(float);
extern void func_00207D00(int, int);
extern void func_00207D90(int slot, int b0, int b2, int b1, int b3);
extern void func_00207E40(int, int, int, int, int, unsigned int, long long);

int func_0020BC50(unsigned char *p, char *tbl, long long g)
{
    int i;
    int base;
    int y;
    int wrapped;
    int step;

    wrapped = 0;
    func_00207D00(1, 0);
    func_00207D90(1, 0x7F, 0x34, 0x17F, 0x92);
    if (p[0x1A] == 1) {
        base = -0x30;
        step = 4;
    } else {
        step = -4;
        base = 0;
    }
    for (i = 0, y = 0x6A; i < 5; i++, y += 0x30) {
        func_00207E40(1, 0x77F0, float_to_int(16.0f * (float)(((*(short *)(p + 0x1C) + (base + y)) >> 1) + 0x790)),
                      0x80, 0x40, 0x40404040, *(long long *)(tbl + p[0x90 + i] * 24));
        func_00207E40(1, 0x7FF0, float_to_int(16.0f * (float)(((*(short *)(p + 0x1C) + (base + y)) >> 1) + 0x790)),
                      0x80, 0x40, 0x40404040, *(long long *)(tbl + p[0x90 + i] * 24 + 8));
    }
    func_00207D90(1, 0, 0, 0x200, 0xE0);
    *(short *)(p + 0x1C) += step;
    if (step < 0) {
        if (*(short *)(p + 0x1C) < -0x2F) {
            if (p[0x19] >= p[0x18] - 1) {
                p[0x19] = 0;
            } else {
                p[0x19] = p[0x19] + 1;
            }
            wrapped = 1;
        }
    } else if (*(short *)(p + 0x1C) >= 0x30) {
        if (p[0x19] == 0) {
            p[0x19] = p[0x18] - 1;
        } else {
            p[0x19] = p[0x19] - 1;
        }
        wrapped = 1;
    }
    func_00207E40(1, 0x77F0, float_to_int(16.0f * (float)(((p[0x17] * 0x30 + 0x6A) >> 1) + 0x790)),
                  0x100, 0x40, 0x20808080, g);
    return wrapped;
}
