// NEARMISS func_0019F730  (vram 0x0019F730, 0x718 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 88.78% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// mwcc233 web-coloring-order permutation, same family wall as committed NEARMISS siblings func_001A1390/func_001A1B80 (they parked at 76%). Pass 1 is fully register-exact (i=s1, j=s2, np=s3, p=s4, manual off4=s5/off40=s6, fp/tbl v0/v1 all match after decl-order tuning); the residual is pass 2: the ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

// SEMANTICS: y-interval hit-scan sibling of func_001A1390/func_001A1B80
// (see docs/FINDINGS.md "4. The uid state words at 0x70003250"). x/z are
// POINT coordinates (D_70003190/D_70003198); only y is an interval
// (D_70003194 vs D_700031A4, sorted to lo/hi).
//   Pass 1: per-uid table walk at 0x70003250 (count 0x7000324C). STOPS
//   at the first entry with flags bit31 clear (break, not continue);
//   skips entries with bit30 set. Stages the area-descriptor kind byte
//   (D_0024D7C0[D_00810700][D_00810701], records 0x28 bytes, +8) through
//   the scratchpad short 0x70003B88, then gates: kind >= 0x5A skip;
//   kind 0x51 requires D_7000324E == 0; kind 0x52 requires
//   D_7000324E == 2; kind 0x53 skipped when D_7000324E == -1.
//   Hull AABB test (x/z point, y interval); on pass walks the prim list
//   (+0x18 count, +0x1C prims) dispatching on the 0xF000 type nibble:
//   0x8000 advance-only, 0x4000 func_001A44B0, 0x2000 func_001A4650,
//   0x1000 func_001A4030 (stops at first hit). On hit: clamp y to the
//   floor-probe result 0x700031B4, clear hit-owner 0x700031D4, OR the
//   kind byte into 0x700030CA low byte, re-clamp y.
//   Pass 2: published CLASS-4 actor list (D_00275B7C/D_00275B84); same
//   uid lookup + AABB + prim dispatch but 0x8000 ALSO calls
//   func_001A44B0 (advance 0x24/0x14), kind gate direct at actor +0x54
//   < 0x51, and hits do NOT stop the prim loop: each hit re-stages
//   (owner = actor, kind byte from +0x54) and sets a local found-flag;
//   the y re-clamp runs once after the loop, guarded by that flag.
// Returns 1 if the segment cleared both passes, 0 if anything hit.

typedef struct Adv48 { int w[12]; } Adv48;   /* 0x30-byte tri record */
typedef struct Adv24 { int w[6]; } Adv24;    /* 0x18-byte tri record */

extern int D_700030B0[];
extern unsigned char **D_0024D7C0[];
extern unsigned char D_00810700[];
extern unsigned char D_00810701[];
extern unsigned char **D_00275B7C;
extern short D_00275B84;
extern int func_001A4030(void *p);
extern int func_001A4650(void *p);
extern int func_001A44B0(void *p);

int func_0019F730(void)
{
    float lo, hi;
    float t;
    int ret;
    int i;
    int j;
    short *np;
    unsigned char *p;
    int off4;
    int off40;
    unsigned char *a;
    short *np2;
    unsigned char *p2;
    int found;
    int k;
    unsigned char **list;
    int hit;

    *(int **)0x700031D0 = D_700030B0;

    hi = *(float *)0x70003194;
    t = *(float *)0x700031A4;
    ret = 1;
    if (!(hi <= t)) {
        lo = t;
    } else {
        lo = hi;
        hi = t;
    }

    for (i = 0, off4 = 0, off40 = 0; i < *(short *)0x7000324C; off4 += 4, off40 += 0x28, i++) {
        int *fp;
        unsigned char *tbl;
        int flags;
        short kind;

        tbl = *(unsigned char **)0x70003250;
        fp = (int *)(tbl + off4 + 4);
        flags = *fp;

        if (!(flags & 0x80000000)) {
            break;
        }
        if (flags & 0x40000000) {
            continue;
        }

        *(short *)0x70003B88 =
            (*(unsigned char **)((char *)D_0024D7C0[D_00810700[0]] + D_00810701[0] * 4))[off40 + 8];
        kind = *(short *)0x70003B88;

        if (kind >= 0x5A) {
            continue;
        }
        if (kind == 0x51 && *(short *)0x7000324E != 0) {
            continue;
        }
        if (kind == 0x52 && *(short *)0x7000324E != 2) {
            continue;
        }
        if (kind == 0x53 && *(short *)0x7000324E == -1) {
            continue;
        }
        {
            unsigned char *hull = tbl + (*fp & 0x3FFFFFFF);

            if (!(*(float *)0x70003190 < *(float *)(hull + 0)) && (*(float *)0x70003190 <= *(float *)(hull + 0xC)) &&
                !(*(float *)0x70003198 < *(float *)(hull + 8)) && (*(float *)0x70003198 <= *(float *)(hull + 0x14)) &&
                !(hi < *(float *)(hull + 4)) && (lo <= *(float *)(hull + 0x10))) {
                np = (short *)(hull + 0x18);
                p = (unsigned char *)np + 4;
                hit = 0;

                for (j = 0; j < *np; j++) {
                    short w = *(short *)p;

                    switch (w & 0xF000) {
                    case 0x8000:
                        if (w & 0x800) {
                            p += 0x24;
                        } else {
                            p += 0x14;
                        }
                        break;
                    case 0x4000:
                        hit = func_001A44B0(p);
                        if (*(short *)p & 0x800) {
                            p += 0x2C;
                        } else {
                            p += 0x18;
                        }
                        break;
                    case 0x2000:
                        hit = func_001A4650(p);
                        p += 0x1C;
                        break;
                    case 0x1000:
                        hit = func_001A4030(p);
                        {
                            int n = p[2];
                            if (*(short *)p & 0x800) {
                                p = (unsigned char *)((Adv48 *)(p + 0x24) + n);
                            } else {
                                p = (unsigned char *)((Adv24 *)(p + 0x14) + n);
                            }
                        }
                        break;
                    }
                    if (hit != 0) {
                        break;
                    }
                }

                if (hit != 0) {
                    ret = 0;
                    *(float *)0x700031A4 = *(float *)0x700031B4;
                    *(void **)0x700031D4 = 0;
                    *(short *)0x700030CA = (short)((*(short *)0x700030CA & 0xFF00) |
                        (*(unsigned char **)((char *)D_0024D7C0[D_00810700[0]] + D_00810701[0] * 4))[off40 + 8]);
                    t = *(float *)0x700031A4;
                    if (!(*(float *)0x70003194 <= t)) {
                        lo = t;
                    } else {
                        hi = t;
                    }
                }
            }
        }
    }

    list = D_00275B7C;

    for (j = 0; j < D_00275B84; j++) {
        a = list[j];
        if (a[0] == 0) {
            continue;
        }
        if ((a[2] & 0x1F) != 4) {
            continue;
        }
        if (*(unsigned char **)0x70003254 == a) {
            continue;
        }
        {
            int uid = (*(unsigned short *)(a + 0xE) >> 8) & 0xFF;

            if (uid == 0xFF) {
                continue;
            }
            {
                unsigned char *tbl = *(unsigned char **)0x70003250;
                int off = *(int *)(tbl + uid * 4 + 4);

                if (off == 0) {
                    continue;
                }
                if (a[0x54] >= 0x51) {
                    continue;
                }
                if (!(uid < *(short *)0x7000324C)) {
                    continue;
                }
                if (uid < 0) {
                    continue;
                }
                {
                    unsigned char *hull = tbl + off;

                    if (!(*(float *)0x70003190 < *(float *)(hull + 0)) && (*(float *)0x70003190 <= *(float *)(hull + 0xC)) &&
                        !(*(float *)0x70003198 < *(float *)(hull + 8)) && (*(float *)0x70003198 <= *(float *)(hull + 0x14)) &&
                        !(hi < *(float *)(hull + 4)) && (lo <= *(float *)(hull + 0x10))) {
                        np2 = (short *)(hull + 0x18);
                        p2 = (unsigned char *)np2 + 4;
                        found = 0;
                        hit = 0;

                        for (k = 0; k < *np2; k++) {
                            switch (*(short *)p2 & 0xF000) {
                            case 0x8000:
                                hit = func_001A44B0(p2);
                                if (*(short *)p2 & 0x800) {
                                    p2 += 0x24;
                                } else {
                                    p2 += 0x14;
                                }
                                break;
                            case 0x4000:
                                hit = func_001A44B0(p2);
                                if (*(short *)p2 & 0x800) {
                                    p2 += 0x2C;
                                } else {
                                    p2 += 0x18;
                                }
                                break;
                            case 0x2000:
                                hit = func_001A4650(p2);
                                p2 += 0x1C;
                                break;
                            case 0x1000:
                                hit = func_001A4030(p2);
                                {
                                    int n = p2[2];
                                    if (*(short *)p2 & 0x800) {
                                        p2 = (unsigned char *)((Adv48 *)(p2 + 0x24) + n);
                                    } else {
                                        p2 = (unsigned char *)((Adv24 *)(p2 + 0x14) + n);
                                    }
                                }
                                break;
                            }
                            if (hit != 0) {
                                found = 1;
                                ret = 0;
                                *(float *)0x700031A4 = *(float *)0x700031B4;
                                *(void **)0x700031D4 = a;
                                *(short *)0x700030CA = (short)((*(short *)0x700030CA & 0xFF00) | a[0x54]);
                            }
                        }

                        if (found != 0) {
                            t = *(float *)0x700031A4;
                            if (!(*(float *)0x70003194 <= t)) {
                                lo = t;
                            } else {
                                hi = t;
                            }
                        }
                    }
                }
            }
        }
    }

    return ret;
}
