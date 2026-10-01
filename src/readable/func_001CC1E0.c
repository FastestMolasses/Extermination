// NEARMISS func_001CC1E0  (vram 0x001CC1E0, 0x1CC bytes) — readable companion C, NOT byte-identical.
//
// objdiff 94.37% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// Two artifacts: the original keeps the byte in a3 across the call to the leaf func_001CBE10
// (intra-TU register analysis; reproducing it needs a static copy of the callee, whose extra .text
// the per-function link cannot take), and the saved-register order of func_001CBA50.
//
// The function links from the asm body in src/func_001CC1E0.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Proportional-width sibling of func_001CBA50: renders a message string into
// the glyph strip (cleared with func_001CCB00(ctx, 0, 0, 0x200, 0x14)) and
// draws it. Each printable byte c adds a glyph cell func_001CC8A0(1, column,
// row, glyph * 30) with glyph c - 0x20 ('$' is glyph 0x89) and advances by
// its width func_001CBE10(c); control bytes add glyph 0 with no width. When
// the strip reaches 0x200 pixels it is drawn with func_001CC3B0(ctx, x, y,
// columns + 1, row + 0x14, width, height, arg7) and restarted at x + width;
// the rest is drawn the same way at the end. row stays 0 throughout.
// The original keeps the byte in a3 across the call to the leaf
// func_001CBE10 (intra-TU register analysis), so the callee is copied here
// as a static of the same TU.
static int func_001CBE10(int id) {
    switch (id) {
    case 0x20: return 5;
    case 0x21: return 4;
    case 0x27: return 4;
    case 0x28: return 8;
    case 0x29: return 8;
    case 0x2C: return 6;
    case 0x2E: return 5;
    case 0x2F: return 8;
    case 0x3A: return 7;
    case 0x3B: return 8;
    case 0x49: return 4;
    case 0x4A: return 7;
    case 0x4D: return 0xC;
    case 0x57: return 0xC;
    case 0x5B: return 8;
    case 0x5C: return 8;
    case 0x5D: return 8;
    case 0x60: return 5;
    case 0x66: return 8;
    case 0x69: return 4;
    case 0x6A: return 6;
    case 0x6C: return 4;
    case 0x6D: return 0xC;
    case 0x72: return 9;
    case 0x77: return 0xC;
    case 0x82: return 6;
    case 0x84: return 8;
    case 0x8B: return 9;
    case 0x91: return 6;
    case 0x92: return 6;
    case 0x93: return 8;
    case 0x94: return 8;
    case 0x9B: return 9;
    case 0xA1: return 4;
    case 0xA6: return 6;
    default: return 9;
    }
}
extern int func_001232E0(unsigned char *s);
extern void func_001CCB00(int ctx, int a, int b, int w, int h);
extern void func_001CC8A0(int a, int col, int row, int glyph);
extern void func_001CC3B0(int ctx, int x, int y, int cols, int rows, int w, int h, int arg7);

void func_001CC1E0(int ctx, int x, int y, int unused, int height, unsigned char *s, int arg7) {
    int col = 0;
    int row = 0;
    int h = 0;
    int w = 0;
    int n = func_001232E0(s);
    unsigned int c;
    int adv;
    int ok;

    func_001CCB00(ctx, 0, 0, 0x200, 0x14);
    while (n != 0) {
        c = *s;
        ok = 0;
        if (c >= 0x20) {
            adv = func_001CBE10(c);
            if (c == 0x24) {
                c = 0x89;
            } else {
                c -= 0x20;
            }
            ok = 1;
        }
        if (!ok) {
            c = 0;
            adv = 0;
        }
        func_001CC8A0(1, col, row, c * 30);
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
