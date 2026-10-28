// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a 94.80% NEARMISS).
// Fixes read from the original instructions:
// - The string length n is declared first and the four counters are zeroed by
//   statements (row, col, h, w) before the length call: that gives the
//   original's saved-register order (w, h, s, x, ctx, n, col, row, y).
// - The dollar-sign mapping is a switch with a default arm (the likely branch
//   for '$'), and the glyph index is scaled in place (c <<= 5) before the call,
//   which keeps it in the argument register and gives the likely branch around
//   the control-byte zeroing.

// Renders a message string into the glyph strip and draws it. The strip is
// cleared with func_001CCB00(ctx, 0, 0, 0x200, 0x10); each byte of s
// (strlen through func_001232E0) adds one 16-pixel glyph cell with
// func_001CC8A0(0, column, row, glyph * 32): printable bytes map to
// byte - 0x20, '$' to glyph 0x87, control bytes to glyph 0. Each cell widens
// the drawn width by advance. When the strip is full (0x200 pixels) it is
// drawn with func_001CBC20(ctx, x, y, width, height, columns, row + 0x10,
// arg7) and restarted at x + width; the rest is drawn the same way at the
// end. row stays 0 throughout.
extern int func_001232E0(unsigned char *s);
extern void func_001CCB00(int ctx, int a, int b, int w, int h);
extern void func_001CC8A0(int a, int col, int row, int glyph);
extern void func_001CBC20(int ctx, int x, int y, int w, int h, int cols, int rows, int arg7);

void func_001CBA50(int ctx, int x, int y, int advance, int height, unsigned char *s, int arg7) {
    int n;
    int col;
    int row;
    int w;
    int h;
    unsigned int c;
    int ok;

    row = 0;
    col = 0;
    h = 0;
    w = 0;
    n = func_001232E0(s);
    func_001CCB00(ctx, 0, 0, 0x200, 0x10);
    while (n != 0) {
        c = *s;
        ok = 0;
        if (c >= 0x20) {
            switch (c) {
            case 0x24:
                c = 0x87;
                break;
            default:
                c -= 0x20;
                break;
            }
            ok = 1;
        }
        if (!ok) {
            c = 0;
        }
        c <<= 5;
        func_001CC8A0(0, col, row, c);
        col += 0x10;
        w += advance;
        if (col >= 0x200) {
            h += height;
            func_001CBC20(ctx, x, y, w, h, col, row + 0x10, arg7);
            x += w;
            row = 0;
            col = 0;
            h = 0;
            w = 0;
            func_001CCB00(ctx, 0, 0, 0x200, 0x10);
        }
        n--;
        s++;
    }
    h += height;
    func_001CBC20(ctx, x, y, w, h, col, row + 0x10, arg7);
}
