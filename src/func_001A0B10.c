// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
// SPAD: 0x70003250 0x7000324C 0x700030CA

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 97.36% NEARMISS).
// Fixes read from the original instructions:
// - The uid table 0x70003250, its count 0x7000324C and the kind word 0x700030CA
//   are relocated scratchpad externs (idiom-32), with D_700031D4 an unsized array.
// - Pass 1 keeps the table-entry pointer (fp) and re-reads it for the hull offset.
// - The 0x1000 prim advance is a record-pointer stride reading the count byte
//   inside each arm.
// - Pass-1 hit block reads the kind word into an int (unmasked) before the
//   low-bound and high-bound loads, then stores (v & 0xFF00) | kind byte.
// - Pass 2 walks the list as a = *list++ with its own counter, prim count and
//   cursor pointers at function scope; hit is cleared in the inner-loop init.
// - Pass 2 reads the uid in two steps (halfword, then >> 8 & 0xFF) and the
//   table offset as a value.
// - Pass 2's bound copy reuses hit as its loop counter (the original does: hit
//   ends at 3, still nonzero, for the rest of the prim loop).
// - Pass 2 ORs the actor kind as (a[0x54] & 0xFF); the re-clamps re-read the
//   scratch bound words in the compare and each arm.
//
// SEMANTICS: segment hit-scan sibling of func_001A2AE0/func_001A32C0/func_001A06A0
// (docs/FINDINGS.md "4. The uid state words at 0x70003250"). Clamps the query
// interval on all three axes (x/y/z, D_70003190..98 lo / 0x700031A0..A8 hi), then:
//   Pass 1: per-uid table walk at 0x70003250 (count 0x7000324C); BREAKS at the
//   first entry with flags bit31 clear; skips bit30 entries; stages the area
//   kind byte D_0024D7C0[D_00810700][D_00810701][uid*0x28+8] through the
//   scratch short 0x70003B88 and gates kinds 0x50..0x59 on the mode short
//   0x7000324E (0x50 always skipped, 0x51 needs mode==0, 0x52 needs mode==2,
//   0x53 skipped when mode==-1). Hull AABB test (x, z, y order), then walks the
//   prim list (+0x18 count, +0x1C prims) dispatching on the 0xF000 type nibble
//   to func_001A5C30 (0x4000) / func_001A50A0 (0x2000) / func_001A4030
//   (0x1000); 0x8000 just advances. On the first hit: re-splats
//   D_70003190[4..6] from [8..10], clears the hit-owner 0x700031D4, ORs the
//   kind byte into 0x700030CA's low byte, re-clamps all 3 axes.
//   Pass 2: published CLASS-4 actor list (D_00275B7C/D_00275B84); same uid
//   lookup + AABB + prim dispatch, kind gate at actor+0x54 < 0x50; hits are
//   processed INSIDE the prim loop (no break): stages the actor pointer at
//   0x700031D4, ORs actor kind into 0x700030CA, re-clamps after the loop.
// Returns 1 if either pass recorded a hit, 0 if the segment cleared both.

typedef struct Adv48 { int w[12]; } Adv48;   /* 0x30-byte tri record */
typedef struct Adv24 { int w[6]; } Adv24;    /* 0x18-byte tri record */

extern int D_700030B0[];
extern void *D_700031D4[];
extern unsigned char *D_70003250[];
extern short D_7000324C[];
extern float D_70003190[];
extern short D_700030CA[];
extern unsigned char **D_0024D7C0[];
extern unsigned char D_00810700[];
extern unsigned char D_00810701[];
extern unsigned char **D_00275B7C;
extern short D_00275B84;
extern int func_001A4030(void *p);
extern int func_001A50A0(void *p);
extern int func_001A5C30(void *p);

int func_001A0B10(void)
{
    float hix, hiy, hiz;
    float lox, loy, loz;
    int ret;
    int i;
    int j;
    unsigned char *cnt;
    unsigned char *p;
    int off4;
    int off28;
    unsigned char **list;
    unsigned char *a;
    int n;
    unsigned char *cnt2;
    unsigned char *p2;
    int found;
    int k;
    int hit;

    *(int **)0x700031D0 = D_700030B0;
    ret = 0;

    lox = *(float *)0x70003190;
    {
        float t = *(float *)0x700031A0;
        if (!(lox <= t)) {
            hix = t;
        } else {
            hix = lox;
            lox = t;
        }
    }

    loy = *(float *)0x70003194;
    {
        float t = *(float *)0x700031A4;
        if (!(loy <= t)) {
            hiy = t;
        } else {
            hiy = loy;
            loy = t;
        }
    }

    loz = *(float *)0x70003198;
    {
        float t = *(float *)0x700031A8;
        if (!(loz <= t)) {
            hiz = t;
        } else {
            hiz = loz;
            loz = t;
        }
    }

    for (i = 0, off4 = 0, off28 = 0; i < D_7000324C[0]; off4 += 4, off28 += 0x28, i++) {
        int *fp;
        unsigned char *uid_table;
        int off;
        short kind;

        uid_table = D_70003250[0];
        fp = (int *)(uid_table + off4 + 4);
        off = *fp;

        if (!(off & 0x80000000)) {
            break;
        }
        if (off & 0x40000000) {
            continue;
        }

        *(short *)0x70003B88 = D_0024D7C0[D_00810700[0]][D_00810701[0]][off28 + 8];
        kind = *(short *)0x70003B88;
        if (!(kind < 0x50)) {
            if (kind >= 0x5A) {
                continue;
            }
            if (kind == 0x50) {
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
        }
        {
            unsigned char *hull = uid_table + (*fp & 0x3FFFFFFF);

            if (!(lox < *(float *)(hull + 0)) && (hix <= *(float *)(hull + 0xC)) &&
                !(loz < *(float *)(hull + 8)) && (hiz <= *(float *)(hull + 0x14)) &&
                !(loy < *(float *)(hull + 4)) && (hiy <= *(float *)(hull + 0x10))) {
                cnt = hull + 0x18;
                p = cnt + 4;
                hit = 0;
                for (j = 0; j < *(short *)cnt; j++) {
                    int ptype = *(short *)p & 0xF000;

                    switch (ptype) {
                    case 0x8000:
                        if (*(short *)p & 0x800) {
                            p += 0x24;
                        } else {
                            p += 0x14;
                        }
                        break;
                    case 0x4000:
                        hit = func_001A5C30(p);
                        if (*(short *)p & 0x800) {
                            p += 0x2C;
                        } else {
                            p += 0x18;
                        }
                        break;
                    case 0x2000:
                        hit = func_001A50A0(p);
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
                    {
                        int m = 0;
                        float *v = D_70003190;
                        do {
                            m += 1;
                            v[4] = v[8];
                            v += 1;
                        } while (m < 3);
                    }
                    D_700031D4[0] = 0;
                    ret = 1;
                    {
                        int kb = D_0024D7C0[D_00810700[0]][D_00810701[0]][off28 + 8];
                        int v = D_700030CA[0];
                        float lo = *(float *)0x70003190;
                        float t = *(float *)0x700031A0;

                        D_700030CA[0] = (short)((v & 0xFF00) | kb);
                        if (!(lo <= t)) {
                            hix = t;
                        } else {
                            lox = t;
                        }
                    }
                    if (!(*(float *)0x70003194 <= *(float *)0x700031A4)) {
                        hiy = *(float *)0x700031A4;
                    } else {
                        loy = *(float *)0x700031A4;
                    }
                    if (!(*(float *)0x70003198 <= *(float *)0x700031A8)) {
                        hiz = *(float *)0x700031A8;
                    } else {
                        loz = *(float *)0x700031A8;
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
            int uid = *(unsigned short *)(a + 0xE);

            uid = (uid >> 8) & 0xFF;
            if (uid == 0xFF) {
                continue;
            }
            {
                unsigned char *tbl = D_70003250[0];
                int off = *(int *)(tbl + uid * 4 + 4);

                if (off == 0) {
                    continue;
                }
                if (!(uid < D_7000324C[0])) {
                    continue;
                }
                if (uid < 0) {
                    continue;
                }
                {
                    unsigned char *hull = tbl + off;

                    if (!(lox < *(float *)(hull + 0)) && (hix <= *(float *)(hull + 0xC)) &&
                        !(loz < *(float *)(hull + 8)) && (hiz <= *(float *)(hull + 0x14)) &&
                        !(loy < *(float *)(hull + 4)) && (hiy <= *(float *)(hull + 0x10)) &&
                        (a[0x54] < 0x50)) {
                        cnt2 = hull + 0x18;
                        p2 = cnt2 + 4;
                        found = 0;
                        for (k = 0, hit = 0; k < *(short *)cnt2; k++) {
                            switch (*(short *)p2 & 0xF000) {
                            case 0x8000:
                                if (*(short *)p2 & 0x800) {
                                    p2 += 0x24;
                                } else {
                                    p2 += 0x14;
                                }
                                break;
                            case 0x4000:
                                hit = func_001A5C30(p2);
                                if (*(short *)p2 & 0x800) {
                                    p2 += 0x2C;
                                } else {
                                    p2 += 0x18;
                                }
                                break;
                            case 0x2000:
                                hit = func_001A50A0(p2);
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
                                {
                                    float *v;

                                    /* the copy reuses hit as its counter (it ends at 3, still nonzero) */
                                    hit = 0;
                                    v = D_70003190;
                                    do {
                                        hit += 1;
                                        v[4] = v[8];
                                        v += 1;
                                    } while (hit < 3);
                                }
                                D_700031D4[0] = a;
                                ret = 1;
                                D_700030CA[0] = (short)((D_700030CA[0] & 0xFF00) | (a[0x54] & 0xFF));
                            }
                        }

                        if (found != 0) {
                            if (!(*(float *)0x70003190 <= *(float *)0x700031A0)) {
                                hix = *(float *)0x700031A0;
                            } else {
                                lox = *(float *)0x700031A0;
                            }
                            if (!(*(float *)0x70003194 <= *(float *)0x700031A4)) {
                                hiy = *(float *)0x700031A4;
                            } else {
                                loy = *(float *)0x700031A4;
                            }
                            if (!(*(float *)0x70003198 <= *(float *)0x700031A8)) {
                                hiz = *(float *)0x700031A8;
                            } else {
                                loz = *(float *)0x700031A8;
                            }
                        }
                    }
                }
            }
        }
    }

    return ret;
}
