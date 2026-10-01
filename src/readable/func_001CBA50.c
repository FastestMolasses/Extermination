// NEARMISS func_001CBA50  (vram 0x001CBA50, 0x1C4 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 94.80% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// Saved-register allocation order and the glyph mapping's branch shape (the original uses a likely
// branch for the dollar sign and a likely branch around the control-byte zeroing); declaration
// orders measured.
//
// The function links from the asm body in src/func_001CBA50.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

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
    int col = 0;
    int row = 0;
    int w = 0;
    int h = 0;
    int n = func_001232E0(s);
    unsigned int c;
    int ok;

    func_001CCB00(ctx, 0, 0, 0x200, 0x10);
    while (n != 0) {
        c = *s;
        ok = 0;
        if (c >= 0x20) {
            if (c == 0x24) {
                c = 0x87;
            } else {
                c -= 0x20;
            }
            ok = 1;
        }
        if (!ok) {
            c = 0;
        }
        func_001CC8A0(0, col, row, c << 5);
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
