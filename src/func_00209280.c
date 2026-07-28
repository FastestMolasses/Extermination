// NEARMISS func_00209280  (vram 0x00209280, 0x5D4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.10% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// GPR saved-register coloring permutation (the documented CW-declaration-order vs mwcc-web-start allocation-ORDER wall) — body/structure/schedule byte-identical, EVERY residual line is DIFF_ARG_MISMATCH on s-reg numbers only: x/y params s1<->s2 (both live across the first call so both webs start at...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// UI meter/grid panel draw (ui_menu_lib). Draws an underline bar + a numeric
// caption (count "/" total from D_00810CB2/D_00810CB7 via the digit formatter
// func_001C5FB0 into the scratch string D_002862C0), then renders D_00810CB2
// items as a grid of w x w glyph cells (12 per row), right-to-left from the
// panel's right edge (odd cells are nudged left by w/8). Cell colors start at
// the RGBA vector 163/54/160/128, stepping toward 255/230/52/128 in 12 steps
// (delta built in scratchpad D_700038A0/B0/C0 by the vector helpers, advanced
// per cell by func_001028B8); each cell's packed color byte comes from
// func_00128250. flag selects the compact (flag==0: bar+caption at x+0x710,
// cell w=8) vs wide (caption at x+0x746, cell w=12) layout. arg3 is the
// 64-bit blend/prim word passed through to the glyph blitter func_00207E40.

extern int float_to_int(float);
extern void func_00102850(void *dst, void *src, float f);
extern void func_001028B8(void *dst, void *a, void *b);
extern void func_001028D0(void *dst, void *a, void *b);
extern void func_00122EF0(char *dst, char *src);
extern void func_00123168(char *dst, char *src);
extern int func_00128250(float f);
extern char *func_001C5FB0(int val, int digits, int pad);
extern void func_001CBA50(int a, int x, int y, int w, int h, void *str, void *glyphs);
extern void func_00207D00(int a, int b);
extern void func_00207E40(int a, int x, int y, int w, int h, unsigned int col, unsigned long long prim);
extern void func_00207F80(int a, int x0, int y0, int x1, int y1, unsigned int color);

extern char *D_0026729C;
extern char D_00265510[];
extern char D_00273568[];
extern char D_002862C0[];
extern unsigned char D_00810C7F;
extern short D_00810CB2;
extern unsigned char D_00810CB7;
extern float D_700038A0[];
extern float D_700038B0[];
extern float D_700038C0[];

void func_00209280(int a, int x, int y, unsigned long long prim, int flag)
{
    int w;
    int row;
    int q;
    int r;
    int yBase;
    int yOff;
    volatile int xStart;
    int n;
    int i;
    int xOff;
    int xo;
    int half;
    unsigned int col;
    float fy;

    func_00207D00(1, 0);
    if (flag == 0) {
        func_00207F80(1,
            float_to_int(16.0f * (float)(x + 0x700)),
            float_to_int(16.0f * (float)(((y + 2) >> 1) + 0x790)),
            float_to_int(16.0f * (float)(x + 0x708)),
            float_to_int(16.0f * (float)(((y + 10) >> 1) + 0x790)),
            0x80CE6000);
        func_001CBA50(1,
            float_to_int(16.0f * (float)(x + 0x70C)) >> 4,
            float_to_int(16.0f * (float)((y >> 1) + 0x790)) >> 4,
            12, 12, D_0026729C, D_00265510);
        w = 8;
    } else {
        w = 12;
    }

    if (D_00810C7F != 0 ? 1 : 0) {
        func_00123168(D_002862C0, func_001C5FB0(D_00810CB2 >> 1, 2, 0));
        func_00122EF0(D_002862C0, D_00273568);
        func_00122EF0(D_002862C0, func_001C5FB0(D_00810CB7 >> 1, 2, 0));
        if (flag == 0) {
            func_001CBA50(1,
                float_to_int(16.0f * (float)(x + 0x710)) >> 4,
                float_to_int(16.0f * (float)(((y + 0x34) >> 1) + 0x790)) >> 4,
                16, 16, D_002862C0, D_00265510);
            yBase = y + 0x10;
            x = x + w * 11;
        } else {
            func_001CBA50(1,
                float_to_int(16.0f * (float)(x + 0x746)) >> 4,
                float_to_int(16.0f * (float)(((y + 0x50) >> 1) + 0x790)) >> 4,
                16, 16, D_002862C0, D_00265510);
            yBase = y + 0x16;
            x = x + 0x26 + w * 11;
        }
        func_00207D00(1, 3);
        row = 0;
        q = D_00810CB2 / 12;
        r = D_00810CB2 % 12;
        if (row < q + 1) {
            xStart = 0;
            yOff = 0;
            half = w >> 3;
            do {
                *(int *)0x700038A0 = 0x43230000;
                *(int *)0x700038A4 = 0x42580000;
                *(int *)0x700038A8 = 0x43200000;
                *(int *)0x700038AC = 0x43000000;
                *(int *)0x700038B0 = 0x437F0000;
                *(int *)0x700038B4 = 0x43660000;
                *(int *)0x700038B8 = 0x42500000;
                *(int *)0x700038BC = 0x43000000;
                func_001028D0(D_700038C0, D_700038B0, D_700038A0);
                func_00102850(D_700038C0, D_700038C0, 12.0f);
                if (row == q) {
                    n = r;
                } else {
                    n = 12;
                }
                i = 0;
                if (i < n) {
                    xOff = xStart;
                    fy = 16.0f * (float)(((yBase + yOff) >> 1) + 0x790);
                    do {
                        if (i & 1) {
                            xo = x - xOff;
                        } else {
                            xo = (x - xOff) - half;
                        }
                        col = func_00128250(*(float *)0x700038A0);
                        col |= func_00128250(*(float *)0x700038A4) << 8;
                        col |= func_00128250(*(float *)0x700038A8) << 16;
                        col |= func_00128250(*(float *)0x700038AC) << 24;
                        func_00207E40(1,
                            float_to_int(16.0f * (float)(xo + 0x700)),
                            float_to_int(fy),
                            w, w, col, prim);
                        func_001028B8(D_700038A0, D_700038A0, D_700038C0);
                        i += 1;
                        xOff += w;
                    } while (i < n);
                }
                yOff += w;
                row += 1;
            } while (row < q + 1);
        }
    }
}
