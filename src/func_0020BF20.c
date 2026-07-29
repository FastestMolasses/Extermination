// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202: under
// 991202 the local/spill-slot layout of this frame differs (0x1F0 frame, the
// eight 16-byte-strided slots at 0x160..0x1D0). 2.3.3 reproduces it exactly,
// so this readable C is byte-identical. objdiff 100.0% vs expected.
//
// Ammo/pickup 4-row HUD counter panel renderer. p is a table of 64-bit glyph
// descriptors (+0x00/0x08 row-1 dim/alt, +0x10/0x18 row-0, +0x20/0x28 row-3,
// +0x30/0x38 row-2); mode bit 0 selects the compact layout (y0=0x96, dim
// colors 0x40404040, alt font D_00265518) vs the full layout (y0=0x128, all
// bright 0x80808080, font D_00265510); sel (1..4) highlights one row bright.
// In mode 2 with sel pointing at the row (0x11/0x12 row0, 0x13/0x14 row1,
// 0x15 row2, 0x16 row3) the icon flashes with pad bit 0x10 (0x70003B64).
// Rows: D_00810CA8, D_00810CAA, D_00810CAE + 100*D_00810CAC, D_00810CB0;
// each nonzero row draws its icon quad (func_00207E40) and its 4-digit count
// via func_001C5FB0/func_00123168 into D_002862C0, blitted by func_001CBA50.
extern int float_to_int(float f);
extern void func_00207D00(int a, int b);
extern void func_00207E40(int a, int x, int y, int w, int h, unsigned int col, long long glyph);
extern int func_001C5FB0(int val, int digits, int pad);
extern void func_00123168(void *dst, int src);
extern void func_001CBA50(int a, int x, int y, int w, int h, void *str, void *glyphs);
extern char D_00265510[];
extern char D_00265518[];
extern char D_002862C0[];
extern unsigned char D_00810CA4;
extern unsigned char D_00810CA6;
extern short D_00810CA8;
extern short D_00810CAA;
extern short D_00810CAC;
extern short D_00810CAE;
extern short D_00810CB0;

void func_0020BF20(char *p, int mode, int sel)
{
    int n;
    int xoff;
    int yoff;
    unsigned int c1;
    char *t0;
    char *t1;
    char *t2;
    char *t3;
    int x0;
    int y0;
    int dy;
    int h;
    unsigned int c0;
    unsigned int c2;
    unsigned int c3;

    if (!(mode & 1)) {
        xoff = 0x2E;
        yoff = 0x10;
        c0 = 0x80808080;
        c1 = c0;
        t0 = D_00265510;
        t1 = D_00265510;
        t2 = D_00265510;
        x0 = 0x17E;
        y0 = 0x128;
        dy = 0x22;
        h = 0x30;
        c2 = c0;
        c3 = c0;
        t3 = D_00265510;
    } else {
        xoff = 0x2E;
        yoff = 0x10;
        c0 = 0x40404040;
        c1 = c0;
        t0 = D_00265518;
        t1 = D_00265518;
        t2 = D_00265518;
        x0 = 0x17E;
        y0 = 0x96;
        dy = 0x22;
        h = 0x30;
        c2 = c0;
        c3 = c0;
        t3 = D_00265518;
        switch (sel) {
        case 0:
            break;
        case 1:
            c0 = 0x80808080;
            t0 = D_00265510;
            break;
        case 2:
            c1 = 0x80808080;
            t1 = D_00265510;
            break;
        case 3:
            c2 = 0x80808080;
            t2 = D_00265510;
            break;
        case 4:
            c3 = 0x80808080;
            t3 = D_00265510;
            break;
        }
    }
    func_00207D00(1, 0);

    n = D_00810CA8;
    if (n != 0) {
        if (mode == 2 && (unsigned int)(sel - 0x11) < 2) {
            if (*(int *)0x70003B64 & 0x10) {
                if (D_00810CA6 == 1) {
                    func_00207E40(1, float_to_int(16.0f * (float)(x0 + 0x700)), float_to_int(16.0f * (float)((y0 >> 1) + 0x790)), h, h, c0, *(long long *)(p + 0x18));
                } else {
                    func_00207E40(1, float_to_int(16.0f * (float)(x0 + 0x700)), float_to_int(16.0f * (float)((y0 >> 1) + 0x790)), h, h, c0, *(long long *)(p + 0x10));
                }
            }
        } else {
            if (D_00810CA6 == 1) {
                func_00207E40(1, float_to_int(16.0f * (float)(x0 + 0x700)), float_to_int(16.0f * (float)((y0 >> 1) + 0x790)), h, h, c0, *(long long *)(p + 0x18));
            } else {
                func_00207E40(1, float_to_int(16.0f * (float)(x0 + 0x700)), float_to_int(16.0f * (float)((y0 >> 1) + 0x790)), h, h, c0, *(long long *)(p + 0x10));
            }
        }
        func_00123168(D_002862C0, func_001C5FB0(n, 4, 1));
        func_001CBA50(1, float_to_int(16.0f * (float)(x0 + xoff + 0x700)) >> 4,
                      float_to_int(16.0f * (float)(((y0 + yoff) >> 1) + 0x790)) >> 4,
                      0x10, 0x10, D_002862C0, t0);
    }

    {
        int n2 = D_00810CAA;
        if (n2 != 0) {
            if (mode == 2 && (unsigned int)(sel - 0x13) < 2) {
                if (*(int *)0x70003B64 & 0x10) {
                    if (D_00810CA6 == 2 || D_00810CA6 == 3) {
                        func_00207E40(1, float_to_int(16.0f * (float)(x0 + 0x700)), float_to_int(16.0f * (float)(((y0 + dy) >> 1) + 0x790)), h, h, c1, *(long long *)(p + 0x8));
                    } else {
                        func_00207E40(1, float_to_int(16.0f * (float)(x0 + 0x700)), float_to_int(16.0f * (float)(((y0 + dy) >> 1) + 0x790)), h, h, c1, *(long long *)(p + 0x0));
                    }
                }
            } else {
                if (D_00810CA6 == 2 || D_00810CA6 == 3) {
                    func_00207E40(1, float_to_int(16.0f * (float)(x0 + 0x700)), float_to_int(16.0f * (float)(((y0 + dy) >> 1) + 0x790)), h, h, c1, *(long long *)(p + 0x8));
                } else {
                    func_00207E40(1, float_to_int(16.0f * (float)(x0 + 0x700)), float_to_int(16.0f * (float)(((y0 + dy) >> 1) + 0x790)), h, h, c1, *(long long *)(p + 0x0));
                }
            }
            func_00123168(D_002862C0, func_001C5FB0(n2, 4, 1));
            func_001CBA50(1, float_to_int(16.0f * (float)(x0 + xoff + 0x700)) >> 4,
                          float_to_int(16.0f * (float)(((yoff + (y0 + dy)) >> 1) + 0x790)) >> 4,
                          0x10, 0x10, D_002862C0, t1);
        }
    }

    {
        int n3 = D_00810CAE + D_00810CAC * 100;
        if (n3 != 0) {
            if (mode == 2 && sel == 0x15) {
                if (*(int *)0x70003B64 & 0x10) {
                    if (D_00810CA6 == 4) {
                        func_00207E40(1, float_to_int(16.0f * (float)(x0 + 0x700)), float_to_int(16.0f * (float)(((y0 + dy * 2) >> 1) + 0x790)), h, h, c2, *(long long *)(p + 0x38));
                    } else {
                        func_00207E40(1, float_to_int(16.0f * (float)(x0 + 0x700)), float_to_int(16.0f * (float)(((y0 + dy * 2) >> 1) + 0x790)), h, h, c2, *(long long *)(p + 0x30));
                    }
                }
            } else {
                if (D_00810CA6 == 4) {
                    func_00207E40(1, float_to_int(16.0f * (float)(x0 + 0x700)), float_to_int(16.0f * (float)(((y0 + dy * 2) >> 1) + 0x790)), h, h, c2, *(long long *)(p + 0x38));
                } else {
                    func_00207E40(1, float_to_int(16.0f * (float)(x0 + 0x700)), float_to_int(16.0f * (float)(((y0 + dy * 2) >> 1) + 0x790)), h, h, c2, *(long long *)(p + 0x30));
                }
            }
            func_00123168(D_002862C0, func_001C5FB0(n3, 4, 1));
            func_001CBA50(1, float_to_int(16.0f * (float)(x0 + xoff + 0x700)) >> 4,
                          float_to_int(16.0f * (float)(((yoff + (y0 + dy * 2)) >> 1) + 0x790)) >> 4,
                          0x10, 0x10, D_002862C0, t2);
        }
    }

    {
        int n4 = D_00810CB0;
        if (n4 != 0) {
            if (mode == 2 && sel == 0x16) {
                if (*(int *)0x70003B64 & 0x10) {
                    if (D_00810CA4 == 2) {
                        func_00207E40(1, float_to_int(16.0f * (float)(x0 + 0x700)), float_to_int(16.0f * (float)(((y0 + dy * 3) >> 1) + 0x790)), h, h, c3, *(long long *)(p + 0x28));
                    } else {
                        func_00207E40(1, float_to_int(16.0f * (float)(x0 + 0x700)), float_to_int(16.0f * (float)(((y0 + dy * 3) >> 1) + 0x790)), h, h, c3, *(long long *)(p + 0x20));
                    }
                }
            } else {
                if (D_00810CA4 == 2) {
                    func_00207E40(1, float_to_int(16.0f * (float)(x0 + 0x700)), float_to_int(16.0f * (float)(((y0 + dy * 3) >> 1) + 0x790)), h, h, c3, *(long long *)(p + 0x28));
                } else {
                    func_00207E40(1, float_to_int(16.0f * (float)(x0 + 0x700)), float_to_int(16.0f * (float)(((y0 + dy * 3) >> 1) + 0x790)), h, h, c3, *(long long *)(p + 0x20));
                }
            }
            func_00123168(D_002862C0, func_001C5FB0(n4, 4, 1));
            func_001CBA50(1, float_to_int(16.0f * (float)(x0 + xoff + 0x700)) >> 4,
                          float_to_int(16.0f * (float)(((yoff + (y0 + dy * 3)) >> 1) + 0x790)) >> 4,
                          0x10, 0x10, D_002862C0, t3);
        }
    }
}
