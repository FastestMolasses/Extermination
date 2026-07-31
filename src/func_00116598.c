// NEARMISS func_00116598  (vram 0x00116598, 0x820 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 62.35% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// eegcc register pressure / dual-pipeline scheduling. Structure is recovered (frame 0xb0 matches exactly, both control-flow halves and all branch shapes line up), but the target keeps five distinct global base addresses live in callee-saved regs across the whole loop while our build spills three of...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern unsigned char D_0027CCC0[];   /* 0x30 x 0x6A entries */
extern unsigned char D_0027CCD4[];   /* == D_0027CCC0 + 0x14 */
extern unsigned char D_0027CCDA[];   /* == D_0027CCC0 + 0x1A */
extern unsigned char D_0027CD0E[];   /* == D_0027CCC0 + 0x4E */
extern unsigned char D_0027E0C0[];   /* 0x30 x 0x78 entries */
extern unsigned char D_0027F740[];
extern unsigned char D_0027C6C4[];
extern unsigned char D_00281AC0[];
extern unsigned char D_00242630[];

extern int func_00117918(int, int, int, int, int);
extern int func_001157F0(int, int, int, int);
extern int func_001179E0(int, unsigned char *);
extern int func_00119080(int, int, int, int);

void func_00116598(void)
{
    unsigned char *p;
    unsigned char *q;
    unsigned char *r;
    int off;
    int i;
    int ah, ax, ay, aw, af;
    int shade;
    int ptr, end;
    int n, hi, lo;
    int v;
    int res;

    r = D_0027CD0E;
    p = D_0027CCC0;
    q = D_0027E0C0;
    off = 0;
    i = 0;
    do {
        if (*(unsigned short *)(p + 0x00) == 1 &&
            *(unsigned short *)(D_0027CCDA + off) != 3 &&
            (unsigned int)*(unsigned short *)(D_0027CCC0 + off + 0x06) < 0x30) {

            q += *(unsigned short *)(D_0027CCC0 + off + 0x06) * 0x78;

            if ((*(unsigned short *)(D_0027CCD4 + off) == 1 ||
                 *(unsigned short *)(p + 0x44) == 1 ||
                 *(unsigned short *)(q + 0x50) == 1) &&
                *(unsigned short *)(q + 0x2E) == 0 &&
                *(unsigned short *)(q + 0x30) == 0) {

                ax = *(unsigned short *)(D_0027CCC0 + off + 0x02);
                ay = *(short *)(D_0027CCC0 + off + 0x36);
                aw = *(unsigned short *)(D_0027CCC0 + off + 0x38);
                af = *(unsigned short *)(D_0027CCC0 + off + 0x3A);
                ah = *(unsigned short *)(D_0027CCC0 + off + 0x40);

                if (*(unsigned short *)(D_0027CCD4 + off) == 1 ||
                    *(unsigned short *)(p + 0x44) == 1) {

                    if (ax < ah) {
                        ah = (ah - ax) + 0x78;
                    } else {
                        ah = 0x78 - (ax - ah);
                    }
                    ah = ah & 0xFFFF;

                    if (*(unsigned short *)(D_0027CCD4 + off) != 0) {
                        if (*(unsigned short *)(D_0027F740 + 0x3A) == 0x3C &&
                            (*(unsigned short *)(D_0027CCC0 + off + 0x3C) & 0xFFF) == 0x78) {
                            if ((*(unsigned short *)(D_0027CCC0 + off + 0x3C) & 0xF000) != 0) {
                                v = (*(unsigned short *)(D_0027CCC0 + off + 0x3C) & 0xFFF) |
                                    ((((*(unsigned short *)(D_0027CCC0 + off + 0x3C) >> 12) - 1) & 0xFFFF) << 12);
                                *(unsigned short *)(D_0027CCC0 + off + 0x3C) = v;
                                *(unsigned short *)(D_0027CCC0 + off + 0x12) += (v & 0xFFF);
                            } else {
                                *(unsigned short *)(D_0027CCC0 + off + 0x3C) =
                                    *(unsigned short *)(D_0027CCC0 + off + 0x3C) | 0x6000;
                            }
                        } else {
                            *(unsigned short *)(D_0027CCC0 + off + 0x12) +=
                                (*(unsigned short *)(D_0027CCC0 + off + 0x3C) & 0xFFF);
                        }

                        shade = 0x80;
                        ptr = *(int *)(D_0027C6C4 + *(unsigned short *)(q + 0x24) * 0xC);
                        if ((ptr & 3) == 0) {
                            end = ptr + *(int *)(ptr + 0x18);
                            *(int *)(D_00281AC0 + 0x10) = ptr;
                            if (end != -1 && (end & 1) == 0) {
                                *(int *)(D_00281AC0 + 0x1C) = end;
                                *(int *)(D_00281AC0 + 0x24) = end;
                                shade = *(unsigned short *)(D_0027CCC0 + off + 0x12);
                                if (shade >= 0xF0) {
                                    shade = (*(unsigned short *)(D_0027CCC0 + off + 0x3C) & 0xFFF) >> 1;
                                    *(unsigned short *)(D_0027CCC0 + off + 0x12) = shade;
                                }
                                shade = *(unsigned char *)(*(int *)(D_00281AC0 + 0x1C) +
                                        *(unsigned short *)(*(int *)(D_00281AC0 + 0x24) +
                                            *(unsigned short *)(D_0027CCC0 + off + 0x10) * 2 + 2) +
                                        (shade >> 2));
                            }
                        }

                        if (*(unsigned short *)(D_0027CCDA + off) == 1) {
                            if (*(unsigned short *)(D_0027CCC0 + off + 0x38) < 0x40) {
                                n = (0x40 - *(unsigned short *)(D_0027CCC0 + off + 0x38)) *
                                    *(unsigned short *)(D_0027CCC0 + off + 0x3A);
                                hi = n / 64;
                                lo = n / 4;
                                ay = ay + (hi * 16 - lo);
                                ax = (ax - hi) & 0xFFFF;
                            } else {
                                n = (*(unsigned short *)(D_0027CCC0 + off + 0x38) - 0x40) *
                                    *(unsigned short *)(D_0027CCC0 + off + 0x3A);
                                hi = n / 64;
                                lo = n / 4;
                                ay = ay + (lo - hi * 16);
                                ax = (ax + hi) & 0xFFFF;
                            }
                            ay = (short)ay;
                            af = 1;
                        }

                        aw = (shade * *(unsigned short *)(p + 0x16)) / 0xFF;
                        aw = (aw + 0x40 - ((*(unsigned short *)(p + 0x16) + 1) >> 1)) & 0xFFFF;
                    }

                    if (*(unsigned short *)(p + 0x44) != 0 &&
                        (*(unsigned short *)(D_0027CCC0 + off + 0x62) & 0x7FFF) != 0) {
                        *(unsigned short *)(D_0027CCC0 + off + 0x62) =
                            (*(unsigned short *)(D_0027CCC0 + off + 0x62) & 0x7FFF) - 1;

                        if ((*(unsigned short *)(D_0027CCC0 + off + 0x60) & 0x80) != 0) {
                            n = *(unsigned short *)(D_0027CCC0 + off + 0x64) -
                                *(unsigned short *)(D_0027CCC0 + off + 0x62);
                            ax = (*(unsigned short *)r -
                                  ((n * (0x100 - *(unsigned short *)(D_0027CCC0 + off + 0x60))) / 10) /
                                      *(unsigned short *)(D_0027CCC0 + off + 0x64)) & 0xFFFF;
                            v = (n * ((-*(unsigned char *)(D_0027CCC0 + off + 0x60)) & 0xFF) * 0xC0) /
                                (*(unsigned short *)(D_0027CCC0 + off + 0x64) * 0x78);
                            ay = (short)(ay - (v - (v / 16) * 16));
                        } else {
                            n = *(unsigned short *)(D_0027CCC0 + off + 0x64) -
                                *(unsigned short *)(D_0027CCC0 + off + 0x62);
                            ax = (*(unsigned short *)r +
                                  ((n * *(unsigned short *)(D_0027CCC0 + off + 0x60)) / 10) /
                                      *(unsigned short *)(D_0027CCC0 + off + 0x64)) & 0xFFFF;
                            v = (n * *(unsigned short *)(D_0027CCC0 + off + 0x60) * 0xC0) /
                                (*(unsigned short *)(D_0027CCC0 + off + 0x64) * 0x78);
                            ay = (short)(ay + (v - (v / 16) * 16));
                        }

                        if ((unsigned int)ax <= 0xC) {
                            ax = 0xC;
                        }
                        if ((unsigned int)ax >= 0xF3) {
                            ax = 0xF3;
                        }
                        *(unsigned short *)(q + 0x58) = ax;
                        if (*(unsigned short *)(D_0027CCC0 + off + 0x62) == 0) {
                            *(unsigned short *)r = ax;
                            *(unsigned short *)(p + 0x44) = 0;
                        }
                    }
                }

                res = func_00117918(ah, ax, ay, aw, af);
                func_001157F0(6, i,
                    (((res * *(unsigned short *)(q + 0x44)) >> 12) * 0x1B9) / 0x1E0, 0);
            }

            if (*(unsigned short *)(D_0027CCDA + off) == 2) {
                if (*(unsigned short *)(D_0027CCC0 + off + 0x46) == 1 ||
                    *(unsigned short *)(D_0027CCC0 + off + 0x48) == 1 ||
                    *(unsigned short *)(q + 0x52) == 1) {

                    if (*(unsigned short *)(D_0027CCC0 + off + 0x46) == 1) {
                        if (*(unsigned short *)(D_0027CCC0 + off + 0x50) !=
                            *(unsigned short *)(D_0027CCC0 + off + 0x52)) {
                            if ((*(unsigned short *)(D_0027CCC0 + off + 0x54) & 0x7FFF) != 0) {
                                *(unsigned short *)(D_0027CCC0 + off + 0x54) =
                                    *(unsigned short *)(D_0027CCC0 + off + 0x54) & 0x7FFF;
                                *(unsigned short *)(D_0027CCC0 + off + 0x2C) = func_00119080(
                                    *(unsigned char *)(D_0027CCC0 + off + 0x50),
                                    *(unsigned char *)(D_0027CCC0 + off + 0x52),
                                    *(unsigned char *)(D_0027CCC0 + off + 0x56),
                                    *(unsigned char *)(D_0027CCC0 + off + 0x54));
                                *(unsigned short *)(D_0027CCC0 + off + 0x54) -= 1;
                            } else {
                                *(unsigned short *)(D_0027CCC0 + off + 0x54) = 0;
                                *(unsigned short *)(D_0027CCC0 + off + 0x2C) =
                                    *(unsigned short *)(D_0027CCC0 + off + 0x50);
                                *(unsigned short *)(D_0027CCC0 + off + 0x46) = 0;
                            }
                        } else {
                            *(unsigned short *)(D_0027CCC0 + off + 0x46) = 0;
                        }
                    }

                    if (*(unsigned short *)(D_0027CCC0 + off + 0x48) == 1) {
                        if (*(unsigned short *)(D_0027CCC0 + off + 0x58) !=
                            *(unsigned short *)(D_0027CCC0 + off + 0x5A)) {
                            if ((*(unsigned short *)(D_0027CCC0 + off + 0x5C) & 0x7FFF) != 0) {
                                *(unsigned short *)(D_0027CCC0 + off + 0x5C) =
                                    *(unsigned short *)(D_0027CCC0 + off + 0x5C) & 0x7FFF;
                                *(unsigned short *)(D_0027CCC0 + off + 0x4C) = func_00119080(
                                    *(unsigned char *)(D_0027CCC0 + off + 0x58),
                                    *(unsigned char *)(D_0027CCC0 + off + 0x5A),
                                    *(unsigned char *)(D_0027CCC0 + off + 0x5E),
                                    *(unsigned char *)(D_0027CCC0 + off + 0x5C));
                                *(unsigned short *)(D_0027CCC0 + off + 0x5C) -= 1;
                            } else {
                                *(unsigned short *)(D_0027CCC0 + off + 0x5C) = 0;
                                *(unsigned short *)(D_0027CCC0 + off + 0x4C) =
                                    *(unsigned short *)(D_0027CCC0 + off + 0x58);
                                *(unsigned short *)(D_0027CCC0 + off + 0x48) = 0;
                            }
                            *(unsigned short *)(D_0027CCC0 + off + 0x32) =
                                *(unsigned short *)(D_00242630 +
                                    ((*(unsigned short *)(D_0027CCC0 + off + 0x4C) >> 2) << 1));
                        } else {
                            *(unsigned short *)(D_0027CCC0 + off + 0x48) = 0;
                        }
                    }

                    res = func_001179E0(i, q);
                    func_001157F0(1, i, res >> 16, (short)res);
                }
            }

            q -= *(unsigned short *)(D_0027CCC0 + off + 0x06) * 0x78;
        }

        i++;
        off += 0x6A;
        p += 0x6A;
        r += 0x6A;
    } while ((unsigned int)i < 0x30);

    i = 0;
    do {
        i++;
        if (*(unsigned short *)(q + 0x50) == 1) {
            *(unsigned short *)(q + 0x50) = 0;
        }
        if (*(unsigned short *)(q + 0x52) == 1) {
            *(unsigned short *)(q + 0x52) = 0;
        }
        q += 0x78;
    } while ((unsigned int)i < 0x30);
}
