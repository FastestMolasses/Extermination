// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// TU_STATIC: func_001CBE10
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a 94.37% NEARMISS).
// Fixes read from the original instructions:
// - func_001CBE10 sits earlier in the same translation unit; compiled as a
//   static same-TU callee (TU_STATIC) the compiler knows it leaves a3 alone,
//   so the byte stays in a3 across the width call (without it: 87%).
// - The string length n is declared first and the four counters are zeroed by
//   statements (row, col, h, w) before the length call: the original's
//   saved-register order (same fix as func_001CBA50).
// - The dollar-sign mapping is a switch with a default arm, and the glyph
//   index is scaled in place (c *= 30) before the call.

// Proportional-width sibling of func_001CBA50: renders a message string into
// the glyph strip (cleared with func_001CCB00(ctx, 0, 0, 0x200, 0x14)) and
// draws it. Each printable byte c adds a glyph cell func_001CC8A0(1, column,
// row, glyph * 30) with glyph c - 0x20 ('$' is glyph 0x89) and advances by
// its width func_001CBE10(c); control bytes add glyph 0 with no width. When
// the strip reaches 0x200 pixels it is drawn with func_001CC3B0(ctx, x, y,
// columns + 1, row + 0x14, width, height, arg7) and restarted at x + width;
// the rest is drawn the same way at the end. row stays 0 throughout.
static int func_001CBE10(int id);  /* same-TU callee, see TU_STATIC */
#include "src/func_001CBE10.c"
extern int func_001232E0(unsigned char *s);
extern void func_001CCB00(int ctx, int a, int b, int w, int h);
extern void func_001CC8A0(int a, int col, int row, int glyph);
extern void func_001CC3B0(int ctx, int x, int y, int cols, int rows, int w, int h, int arg7);

void func_001CC1E0(int ctx, int x, int y, int unused, int height, unsigned char *s, int arg7) {
    int n;
    int col;
    int row;
    int w;
    int h;
    unsigned int c;
    int adv;
    int ok;

    row = 0;
    col = 0;
    h = 0;
    w = 0;
    n = func_001232E0(s);

    func_001CCB00(ctx, 0, 0, 0x200, 0x14);
    while (n != 0) {
        c = *s;
        ok = 0;
        if (c >= 0x20) {
            adv = func_001CBE10(c);
            switch (c) {
            case 0x24:
                c = 0x89;
                break;
            default:
                c -= 0x20;
                break;
            }
            ok = 1;
        }
        if (!ok) {
            c = 0;
            adv = 0;
        }
        c *= 30;
        func_001CC8A0(1, col, row, c);
        col += adv;
        w += adv;
        if (col >= 0x200) {
            h += height;
            func_001CC3B0(ctx, x, y, col + 1, row + 0x14, w, h, arg7);
            x += w;
            row = 0;
            col = 0;
            h = 0;
            w = 0;
            func_001CCB00(ctx, 0, 0, 0x200, 0x14);
        }
        n--;
        s++;
    }
    h += height;
    func_001CC3B0(ctx, x, y, col + 1, row + 0x14, w, h, arg7);
}
