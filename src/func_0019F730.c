// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
// SPAD: 0x70003250 0x70003194 0x700031A4 0x700030CA

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 88.78% NEARMISS).
// Fixes read from the original instructions:
// - The scratchpad words 0x70003250 (uid table), 0x70003194 / 0x700031A4 (y
//   interval) and 0x700030CA (kind word) are relocated externs (idiom-32),
//   plus D_700031D4 as an unsized array; only then may the scheduler move the
//   table and kind-word loads above the hit-block stores, as the original does.
// - The area-descriptor kind byte is plain triple indexing
//   D_0024D7C0[area][sub][off40 + 8].
// - The 0x1000 prim advance reads the count byte p[2] inside each arm.
// - Pass 2 walks the actor list by pointer (a = *list++) with its own counter
//   declared after list; hit is cleared in the inner loop's init.
// - Both y re-clamps re-read 0x700031A4 in the compare and in each arm.
// - Pass 2 ORs the actor kind as (a[0x54] & 0xFF) into the masked kind word.
//
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
extern void *D_700031D4[];
extern unsigned char *D_70003250[];
extern float D_70003194[];
extern float D_700031A4[];
extern short D_700030CA[];
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
    unsigned char **list;
    unsigned char *a;
    int n;
    short *np2;
    unsigned char *p2;
    int found;
    int k;
    int hit;

    *(int **)0x700031D0 = D_700030B0;

    hi = D_70003194[0];
    t = D_700031A4[0];
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

        tbl = D_70003250[0];
        fp = (int *)(tbl + off4 + 4);
        flags = *fp;

        if (!(flags & 0x80000000)) {
            break;
        }
        if (flags & 0x40000000) {
            continue;
        }

        *(short *)0x70003B88 =
            D_0024D7C0[D_00810700[0]][D_00810701[0]][off40 + 8];
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
                        if (*(short *)p & 0x800) {
                            p = (unsigned char *)((Adv48 *)(p + 0x24) + p[2]);
                        } else {
                            p = (unsigned char *)((Adv24 *)(p + 0x14) + p[2]);
                        }
                        break;
                    }
                    if (hit != 0) {
                        break;
                    }
                }

                if (hit != 0) {
                    ret = 0;
                    D_700031A4[0] = *(float *)0x700031B4;
                    D_700031D4[0] = 0;
                    D_700030CA[0] = (short)((D_700030CA[0] & 0xFF00) |
                        D_0024D7C0[D_00810700[0]][D_00810701[0]][off40 + 8]);
                    if (!(D_70003194[0] <= D_700031A4[0])) {
                        lo = D_700031A4[0];
                    } else {
                        hi = D_700031A4[0];
                    }
                }
            }
        }
    }

    list = D_00275B7C;

    for (n = 0; n < D_00275B84; n++) {
        a = *list++;
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
                unsigned char *tbl = D_70003250[0];
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
                        for (k = 0, hit = 0; k < *np2; k++) {
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
                                if (*(short *)p2 & 0x800) {
                                    p2 = (unsigned char *)((Adv48 *)(p2 + 0x24) + p2[2]);
                                } else {
                                    p2 = (unsigned char *)((Adv24 *)(p2 + 0x14) + p2[2]);
                                }
                                break;
                            }
                            if (hit != 0) {
                                found = 1;
                                ret = 0;
                                D_700031A4[0] = *(float *)0x700031B4;
                                D_700031D4[0] = a;
                                D_700030CA[0] = (short)((D_700030CA[0] & 0xFF00) | (a[0x54] & 0xFF));
                            }
                        }

                        if (found != 0) {
                            if (!(D_70003194[0] <= D_700031A4[0])) {
                                lo = D_700031A4[0];
                            } else {
                                hi = D_700031A4[0];
                            }
                        }
                    }
                }
            }
        }
    }

    return ret;
}
