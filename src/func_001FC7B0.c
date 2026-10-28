// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 61.16% NEARMISS).
// Fixes read from the original instructions:
//  - the 0x80-byte glyph template is one struct copy (`buf = D_00264C10;`, a
//    128-byte union as in func_001FCBD0), not a hand-written quadword loop;
//  - the walk is a plain `while (i < len)` loop (no early return on an empty
//    string), and the newline test lives inside one skip branch shared by
//    control chars, 0x80, 0xA0..0xDF and 0xF0..0xFF; the upper bound of the
//    0xA0 range is spelled `c <= 0xDF`;
//  - the character is an `unsigned char` local (its 0x81 -> space store is a
//    byte-typed constant), the run counter is zeroed before the memset call,
//    and the recursion passes `str + (i + 1)`;
//  - local declaration order (i, len, n, base, c) gives the original's
//    saved-register assignment (the "register wall" was these source bugs).
//
// Recursive line-layout / text-wrap glyph emitter. arg0 = x base, arg1 = y base,
// str = the text string, cfg = a layout-config record (cfg[1] = glyph stride,
// (cfg[4] + cfg[2]) >> 1 = newline line advance, cfg[5] = the draw param passed as
// func_001CC1E0's 7th arg). Copies a 0x80-byte template from D_00264C10 into a
// local buffer; len = strlen(str) (func_001232E0). Walks the string by index i:
//   * c == 0x0A (newline): recurse with arg1 advanced by the line advance onto
//     str + i + 1, then return.
//   * c < 0x20 (other control), c == 0x80, 0xA0 <= c < 0xE0, or c >= 0xF0: skip.
//   * otherwise (0x20..0x9F except 0x80, or 0xE0..0xEF) start a run:
//     base = arg0 + i * cfg[1]; clear the buffer (func_00121A28(buf, 0, 0x80));
//     copy run chars into it while they stay in [0x20, 0x100) and i < len,
//     turning a 0x81 lead byte plus its trail byte into one space (0x20); if any
//     chars were copied, draw via
//     func_001CC1E0(1, base + 0x700, arg1 + 0x790, 0xA, 0x14, buf, cfg[5]).
extern void func_00121A28(void *dst, int val, int size);
extern unsigned int func_001232E0(unsigned char *s);
extern int func_001CC1E0(int a, int b, int c, int d, int e, char *f, int g);

typedef unsigned __int128 u128;
typedef union Buf128 {
    u128 q[8];
    char b[0x80];
} Buf128;
extern Buf128 D_00264C10;

void func_001FC7B0(int arg0, int arg1, unsigned char *str, int *cfg) {
    Buf128 buf;
    int i;
    unsigned int len;
    int n;
    int base;
    unsigned char c;
    char *dst;

    buf = D_00264C10;
    len = func_001232E0(str);
    i = 0;
    while (i < len) {
        c = str[i];
        if (c < 0x20 || c == 0x80 || (c >= 0xA0 && c <= 0xDF) || c >= 0xF0) {
            if (c == 0xA) {
                func_001FC7B0(arg0, arg1 + ((cfg[4] + cfg[2]) >> 1), str + (i + 1), cfg);
                return;
            }
            i++;
        } else {
            base = arg0 + i * cfg[1];
            n = 0;
            func_00121A28(buf.b, 0, 0x80);
            dst = buf.b;
            while (c >= 0x20 && c < 0x100 && i < len) {
                if (c == 0x81) {
                    i++;
                    c = 0x20;
                }
                i++;
                *dst++ = c;
                c = str[i];
                n++;
            }
            if (n > 0) {
                func_001CC1E0(1, base + 0x700, arg1 + 0x790, 0xA, 0x14, buf.b, cfg[5]);
            }
        }
    }
}
