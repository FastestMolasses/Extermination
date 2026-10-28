// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// Scrolling step of the 9-row text list p (called by func_00214020):
// same scheme as func_0020BC50 with 12-unit rows. Clips to the list window,
// draws rows p[0x90..0x98] through func_001FCF60(entry, 0x64, y) at
// y = 0x47 + 12*i + scroll offset p+0x1C (-12 when scrolling down), moves
// the offset by 1 or -1 and, past a full row, steps the ring head p[0x19]
// with wrap-around over p[0x18] and returns 1 (else 0). Then draws the
// two-half cursor highlight (x 0x7000 and 0x8000) at row p[0x17].

extern int float_to_int(float);
extern void func_001FCF60(int a0, int a1, int a2);
extern void func_00207D00(int, int);
extern void func_00207D90(int slot, int b0, int b2, int b1, int b3);
extern void func_00207E40(int, int, int, int, int, unsigned int, long long);

int func_00213CC0(unsigned char *p)
{
    short step;
    int i;
    int base;
    int wrapped;
    int y;

    wrapped = 0;
    func_00207D90(1, 0x46, 0x46, 0x1EA, 0xA5);
    if (p[0x1A] == 1) {
        base = -0xC;
        step = 1;
    } else {
        base = 0;
        step = -1;
    }
    for (i = 0, y = 0x47; i < 9; i++, y += 0xC) {
        func_001FCF60(p[0x90 + i], 0x64, *(short *)(p + 0x1C) + (base + y));
    }
    func_00207D90(1, 0, 0, 0x200, 0xE0);
    *(short *)(p + 0x1C) += step;
    if (step < 0) {
        if (*(short *)(p + 0x1C) < -0xB) {
            if (p[0x19] >= p[0x18] - 1) {
                p[0x19] = 0;
            } else {
                p[0x19] = p[0x19] + 1;
            }
            wrapped = 1;
        }
    } else if (*(short *)(p + 0x1C) >= 0xC) {
        if (p[0x19] == 0) {
            p[0x19] = p[0x18] - 1;
        } else {
            p[0x19] = p[0x19] - 1;
        }
        wrapped = 1;
    }
    func_00207D00(1, 0);
    func_00207E40(1, 0x7000, float_to_int(16.0f * (float)(((p[0x17] * 0x18 + 0x88) >> 1) + 0x790)),
                  0x100, 0x40, 0x80808080, 0x20044585A1322100LL);
    func_00207E40(1, 0x8000, float_to_int(16.0f * (float)(((p[0x17] * 0x18 + 0x88) >> 1) + 0x790)),
                  0x100, 0x40, 0x80808080, 0x20044705A1322140LL);
    return wrapped;
}
