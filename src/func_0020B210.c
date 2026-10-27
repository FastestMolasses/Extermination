// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 99.70% NEARMISS).
// Fixes read from the original instructions:
//  - the ring-refill loop and all three draw loops share ONE counter (i),
//    declared right after n; separate counters colored the tri-state loop's
//    registers in a different order;
//  - each row-vs-selection test reads the row byte into a local (`row`)
//    first, which puts the row value as the first compare operand;
//  - the selected-row marker forms its record offset in a temporary
//    (`recofs = entry * 24`, then + tbl + 0x10), and the final 4 -> 3 check
//    reads the ring entry into `selrow` before comparing it with p[0x1B].

//
// Squad/inventory row-select renderer + input handler. p is the menu state
// (p[0x17] cursor, p[0x18] item count, p[0x19] ring head, p[0x1A] out-event,
// p[0x12]/p[0x1E] selection base, p+0x50 ring source, p+0x90 visible rows);
// tbl is a table of 24-byte records holding three 64-bit glyph descriptors
// (+0 label, +8 label2, +0x10 selected-row glyph); g is the 64-bit glyph for
// the cursor row; flags is the mode mask (0x400 input-lock, 0x1F0 compare
// modes, 0x20 tri-state mode, 0x40 binary mode, 0x200 no-commit, 8 numeric
// readout). Phase 1: on up (0x1000) / down (0x4000) input moves the cursor
// with edge behavior (wrap event spC0=1 -> p[0x1A]) and click feedback
// func_0020CDA0. Phase 2: refills the 5 visible rows from the ring buffer.
// Phase 3: draws up to 4 rows (two glyph quads each at x=0x77F0/0x7FF0,
// y stepped 0x30 raster units), coloring the row by cursor match and by
// comparing the row value against the selection st (>= in tri-state mode 2,
// == otherwise): bright 0x80208080 hit / 0x80808080 cursor miss,
// dim 0x40208080 hit / 0x40404040 non-cursor miss; in plain mode (no 0x1F0)
// also formats the per-row number (D_00810700 table) via
// func_001C5FB0/func_00123168 into spE0 and blits it with func_001CBA50.
// Phase 4: commits the selection to D_002821B8 (mode 0x40: 2 -> base+2,
// 0 -> 0x3D, else 0x3E; mode 0x20: base+row with the 0xC/0xD/0xE ->
// 0x3C promotion when all three D_00810C70..72 flags sum to 3), draws the
// cursor row highlight (glyph g) and the selected-row marker, and drops
// D_00282240 4 -> 3 when the cursor left the committed row. Returns the
// wrap event (cleared when fewer than 5 items).
//
// The int[4] spB0/spC0/spE0 cells reproduce the original's 16-byte-aligned
// stack slots and its kept store/reload sequences; the t0 temp forces the
// full-word lw in p[0x1A] = spC0[0]. One counter i serves the ring refill and
// all three row loops; the row value is read into `row` before each == / >=
// test against st, and the selected-row record offset / ring entry go through
// `recofs` / `selrow` temporaries (these spellings set the original's operand
// order and saved-register assignment).

extern int float_to_int(float);
extern void func_00123168(void *, int);
extern int func_001C5FB0(int, int, int);
extern void func_001CBA50(int, int, int, int, int, void *, void *);
extern void func_00207D00(int, int);
extern void func_00207E40(int, int, int, int, int, unsigned int, long long);
extern int func_0020BEF0(unsigned char *);
extern void func_0020CDA0(int);

extern int D_002821B4;
extern int D_002821B8;
extern int D_00282240;
extern char D_00265510[];
extern char D_00265518[];
extern unsigned char D_00810700[];
extern unsigned char D_00810C70;
extern unsigned char D_00810C71;
extern unsigned char D_00810C72;
extern unsigned short D_00810E78;

int func_0020B210(unsigned char *p, char *tbl, long long g, int flags)
{
    int spB0[4];
    int spC0[4];
    int spE0[4];
    int t0;
    int n;
    int i;
    int src;
    int k;
    unsigned char st;
    unsigned int color;
    char *ptr2;
    float f;
    int sel;
    int recofs;
    int selrow;
    int row;

    p[0x1A] = 0;
    n = p[0x18];
    k = 0;
    spB0[0] = 0;
    spC0[0] = 0;
    if (n != 0 && !(flags & 0x400)) {
        if (D_00810E78 & 0x1000) {
            if (p[0x17] == 0) {
                spC0[0] = 1;
                t0 = spC0[0];
                p[0x1A] = t0;
                if ((int)p[0x18] > 4) {
                    func_0020CDA0(n);
                }
            } else {
                func_0020CDA0(n);
                p[0x17] = p[0x17] - 1;
            }
        } else if (D_00810E78 & 0x4000) {
            if ((int)n < 4) {
                int t = n - 1;
                if ((int)p[0x17] >= t) {
                    p[0x17] = t;
                } else {
                    int u = p[0x17];
                    p[0x17] = u + 1;
                    func_0020CDA0(u);
                }
            } else {
                if ((int)p[0x17] >= 3) {
                    p[0x17] = 3;
                    p[0x1A] = 2;
                    spC0[0] = 1;
                    if ((int)p[0x18] > 4) {
                        func_0020CDA0(n);
                    }
                } else {
                    p[0x17] = p[0x17] + 1;
                    func_0020CDA0(n);
                }
            }
        }
    }

    src = p[0x19];
    flags = flags & ~0x400;
    for (i = 0; i < (int)p[0x18] && i < 5; i++) {
        p[0x90 + i] = p[0x50 + src];
        src += 1;
        if (src >= (int)p[0x18]) {
            src = 0;
        }
    }

    func_00207D00(1, 0);

    if (flags & 0x1F0) {
        if (p[0x12] == 0xFF) {
            st = 0xFF;
        } else {
            st = p[0x12] - *(unsigned char *)(p + 0x1E);
        }
        if (flags & 0x20) {
            for (i = 0; i < (int)p[0x18] && i < 4; i++) {
                if (st == 2) {
                    if (p[0x17] == i) {
                        row = p[0x90 + i];
                        if (row >= st) {
                            color = 0x80208080;
                        } else {
                            color = 0x80808080;
                        }
                    } else {
                        row = p[0x90 + i];
                        if (row >= st) {
                            color = 0x40208080;
                        } else {
                            color = 0x40404040;
                        }
                    }
                } else {
                    if (p[0x17] == i) {
                        row = p[0x90 + i];
                        if (row == st) {
                            color = 0x80208080;
                        } else {
                            color = 0x80808080;
                        }
                    } else {
                        row = p[0x90 + i];
                        if (row == st) {
                            color = 0x40208080;
                        } else {
                            color = 0x40404040;
                        }
                    }
                }
                f = 16.0f * (float)(((k + 0x6A) >> 1) + 0x790);
                func_00207E40(1, 0x77F0, float_to_int(f), 0x80, 0x40, color,
                              *(long long *)(tbl + p[0x90 + i] * 24));
                func_00207E40(1, 0x7FF0, float_to_int(f), 0x80, 0x40, color,
                              *(long long *)(tbl + p[0x90 + i] * 24 + 8));
                k += 0x30;
            }
        } else {
            for (i = 0; i < (int)p[0x18] && i < 4; i++) {
                if (p[0x17] == i) {
                    row = p[0x90 + i];
                    if (row == st) {
                        color = 0x80208080;
                    } else {
                        color = 0x80808080;
                    }
                } else {
                    row = p[0x90 + i];
                    if (row == st) {
                        color = 0x40208080;
                    } else {
                        color = 0x40404040;
                    }
                }
                f = 16.0f * (float)(((k + 0x6A) >> 1) + 0x790);
                func_00207E40(1, 0x77F0, float_to_int(f), 0x80, 0x40, color,
                              *(long long *)(tbl + p[0x90 + i] * 24));
                func_00207E40(1, 0x7FF0, float_to_int(f), 0x80, 0x40, color,
                              *(long long *)(tbl + p[0x90 + i] * 24 + 8));
                k += 0x30;
            }
        }
    } else {
        for (i = 0; i < (int)p[0x18] && i < 4; i++) {
            if (p[0x17] == i) {
                color = 0x80808080;
                ptr2 = D_00265510;
            } else {
                color = 0x40404040;
                ptr2 = D_00265518;
            }
            f = 16.0f * (float)(((k + 0x6A) >> 1) + 0x790);
            func_00207E40(1, 0x77F0, float_to_int(f), 0x80, 0x40, color,
                          *(long long *)(tbl + p[0x90 + i] * 24));
            func_00207E40(1, 0x7FF0, float_to_int(f), 0x80, 0x40, color,
                          *(long long *)(tbl + p[0x90 + i] * 24 + 8));
            if (flags & 8) {
                func_00123168(&spE0,
                              func_001C5FB0(*(D_00810700 + p[0x90 + i] + *(short *)(p + 0x1E) + 0x564), 2, 0));
                func_001CBA50(1, 0x7AF,
                              float_to_int(16.0f * (float)(((k + 0x86) >> 1) + 0x790)) >> 4,
                              0xC, 0xC, &spE0, ptr2);
            }
            k += 0x30;
        }
    }

    if (p[0x18] != 0) {
        if (!(flags & 0x200)) {
            D_002821B4 = 1;
            if (flags & 0x40) {
                unsigned char w = p[0x90 + p[0x17]];
                if (w == 2) {
                    D_002821B8 = *(short *)(p + 0x1E) + (w & 0xFF);
                } else {
                    if (w == 0) {
                        D_002821B8 = 0x3D;
                    } else {
                        D_002821B8 = 0x3E;
                    }
                }
            } else if (flags & 0x20) {
                D_002821B8 = *(short *)(p + 0x1E) + p[0x90 + p[0x17]];
                if (D_002821B8 == 0xC || (unsigned int)(D_002821B8 - 0xD) < 2U) {
                    spB0[0] = D_00810C70 + D_00810C71;
                    spB0[0] = spB0[0] + D_00810C72;
                    if (spB0[0] == 3) {
                        D_002821B8 = 0x3C;
                    }
                }
            } else {
                D_002821B8 = *(short *)(p + 0x1E) + p[0x90 + p[0x17]];
            }
        }
        sel = func_0020BEF0(p);
        func_00207D00(1, 0);
        func_00207E40(1, 0x77F0,
                      float_to_int(16.0f * (float)(((p[0x17] * 0x30 + 0x6A) >> 1) + 0x790)),
                      0x100, 0x40, 0x20808080, g);
        if (!(flags & 0x40)) {
            if (flags & 0x20) {
                if (spB0[0] == 0) {
                    recofs = p[0x50 + sel] * 24;
                    func_00207E40(1, 0x89F0, 0x83E0, 0x40, 0x40, 0x40808080,
                                  *(long long *)(recofs + (int)tbl + 0x10));
                } else if (spB0[0] == 3) {
                    recofs = p[0x50 + sel] * 24;
                    func_00207E40(1, 0x89F0, 0x83E0, 0x40, 0x40, 0x40808080,
                                  *(long long *)(recofs + (int)tbl + 0x10));
                }
            } else {
                recofs = p[0x50 + sel] * 24;
                func_00207E40(1, 0x89F0, 0x83E0, 0x40, 0x40, 0x40808080,
                              *(long long *)(recofs + (int)tbl + 0x10));
            }
        }
        if (D_00282240 == 4) {
            selrow = p[0x50 + sel];
            if (selrow != p[0x1B]) {
                D_00282240 = 3;
            }
        }
    } else {
        D_002821B4 = 0;
    }

    if ((int)p[0x18] >= 5) {
        return spC0[0];
    }
    spC0[0] = 0;
    return spC0[0];
}
