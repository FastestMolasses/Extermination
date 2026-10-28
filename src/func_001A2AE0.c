// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
// SPAD: 0x70003250 0x7000324C 0x700030CA

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 77.20% NEARMISS).
// Fixes read from the original instructions (the old body was wrong in places):
// - Rewritten in the byte-matched sibling func_001A0B10's spelling: relocated
//   scratchpad externs for the uid table 0x70003250, its count 0x7000324C and the
//   kind word 0x700030CA; function-scope counters/cursors; `a = *list++` walk.
// - Pass 1 BREAKS at the first entry with flags bit31 clear (was continue).
// - Kind gates are inverted: entries/actors with kind < 0x5A are SKIPPED (the old
//   C skipped >= 0x5A). In pass 2 the actor kind gate comes before the uid lookup.
// - The prim dispatch has no default: 0x8000 just advances, 0x4000 alone calls
//   func_001A44B0 (pass 1) / func_001A5C30 (pass 2); other types do not advance.
// - AABB test order is x, y, z; the 0x1000 advance is a record-pointer stride.
// - Pass 1 keeps the entry pointer and re-reads the word for the hull offset.
// - Pass 1 hit block stores the floor-probe result 0x700031B4 into the HIGH y
//   bound word 0x700031A4 (D_700031A4 extern), not into the low y local; then
//   clears 0x700031D4, ORs the kind byte, and re-clamps.
// - All three initial clamps share the x-axis if/else shape; ret starts at 0
//   before them. Pass 2 has no deferred re-clamp flag: it breaks on the first hit.
//
// SEMANTICS: segment hit-scan sibling of func_001A0B10/func_001A32C0/
// func_001A3980/func_001A06A0 (docs/FINDINGS.md "4. The uid state words at
// 0x70003250"). Clamps the query interval on all three axes (x/y/z,
// D_70003190..98 lo / 0x700031A0..A8 hi), then:
//   Pass 1: per-uid table walk at 0x70003250 (count 0x7000324C); breaks at the
//   first entry with flags bit31 clear, skips bit30 entries and bit29 entries
//   while the mode short 0x7000324E is 0, and skips area-descriptor kinds
//   (D_0024D7C0[D_00810700][D_00810701][uid*0x28+8]) below 0x5A. Hull AABB test
//   on all 3 axes; on pass, walks the prim list (+0x18 count, +0x1C prims)
//   dispatching on the 0xF000 type nibble to func_001A44B0 (0x4000) /
//   func_001A4650 (0x2000) / func_001A4030 (0x1000); 0x8000 just advances. On
//   the first hit: copy the floor-probe result 0x700031B4 into the high y bound
//   0x700031A4, clear the hit-owner 0x700031D4, OR the kind byte into
//   0x700030CA's low byte, re-clamp all 3 axes.
//   Pass 2: published CLASS-4 actor list (D_00275B7C/D_00275B84); skips the
//   actor at 0x70003254 and actors whose kind (+0x54) is below 0x5A; same uid
//   lookup + AABB + prim dispatch (0x4000 -> func_001A5C30, 0x2000 ->
//   func_001A50A0, 0x1000 -> func_001A4030). On the first hit: re-splats
//   D_70003190[4..6] from [8..10], stages the actor pointer at 0x700031D4, ORs
//   the actor kind into 0x700030CA, re-clamps all 3 axes.
// Returns 1 if either pass recorded a hit, 0 otherwise.

typedef struct Adv48 { int w[12]; } Adv48;   /* 0x30-byte tri record */
typedef struct Adv24 { int w[6]; } Adv24;    /* 0x18-byte tri record */

extern int D_700030B0[];
extern void *D_700031D4[];
extern unsigned char *D_70003250[];
extern short D_7000324C[];
extern float D_70003190[];
extern float D_700031A4[];
extern short D_700030CA[];
extern unsigned char **D_0024D7C0[];
extern unsigned char D_00810700[];
extern unsigned char D_00810701[];
extern unsigned char **D_00275B7C;
extern short D_00275B84;
extern int func_001A4030(void *p);
extern int func_001A44B0(void *p);
extern int func_001A4650(void *p);
extern int func_001A50A0(void *p);
extern int func_001A5C30(void *p);

int func_001A2AE0(void)
{
    float hix, hiy, hiz;
    float lox, loy, loz;
    int i;
    int j;
    int ret;
    unsigned char *cnt;
    unsigned char *p;
    int off4;
    int off28;
    unsigned char *a;
    unsigned char **list;
    int n;
    unsigned char *cnt2;
    unsigned char *p2;
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

        uid_table = D_70003250[0];
        {
            int *e = (int *)(uid_table + off4);
            fp = e + 1;
            off = e[1];
        }

        if (!(off & 0x80000000)) {
            break;
        }
        if (off & 0x40000000) {
            continue;
        }
        if ((off & 0x20000000) && *(short *)0x7000324E == 0) {
            continue;
        }
        if (D_0024D7C0[D_00810700[0]][D_00810701[0]][off28 + 8] < 0x5A) {
            continue;
        }
        {
            unsigned char *hull = uid_table + (*fp & 0x3FFFFFFF);

            if (!(lox < *(float *)(hull + 0)) && (hix <= *(float *)(hull + 0xC)) &&
                !(loy < *(float *)(hull + 4)) && (hiy <= *(float *)(hull + 0x10)) &&
                !(loz < *(float *)(hull + 8)) && (hiz <= *(float *)(hull + 0x14))) {
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
                    {
                        int kb;
                        int v;
                        float lo;
                        float t;

                        D_700031A4[0] = *(float *)0x700031B4;
                        D_700031D4[0] = 0;
                        kb = D_0024D7C0[D_00810700[0]][D_00810701[0]][off28 + 8];
                        v = D_700030CA[0];
                        ret = 1;
                        lo = *(float *)0x70003190;
                        t = *(float *)0x700031A0;

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
        if (a[0x54] < 0x5A) {
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
                if (!(uid < D_7000324C[0])) {
                    continue;
                }
                if (uid < 0) {
                    continue;
                }
                {
                    unsigned char *hull = tbl + off;

                    if (!(lox < *(float *)(hull + 0)) && (hix <= *(float *)(hull + 0xC)) &&
                        !(loy < *(float *)(hull + 4)) && (hiy <= *(float *)(hull + 0x10)) &&
                        !(loz < *(float *)(hull + 8)) && (hiz <= *(float *)(hull + 0x14))) {
                        cnt2 = hull + 0x18;
                        p2 = cnt2 + 4;
                        hit = 0;
                        for (k = 0; k < *(short *)cnt2; k++) {
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
                                break;
                            }
                        }

                        if (hit != 0) {
                            {
                                float *v = D_70003190;
                                int m = 0;
                                do {
                                    m += 1;
                                    v[4] = v[8];
                                    v += 1;
                                } while (m < 3);
                            }
                            D_700031D4[0] = a;
                            ret = 1;
                            {
                                int v = D_700030CA[0];
                                float lo = *(float *)0x70003190;
                                float t = *(float *)0x700031A0;

                                D_700030CA[0] = (short)((v & 0xFF00) | (a[0x54] & 0xFF));
                                if (!(lo <= t)) {
                                    hix = t;
                                } else {
                                    lox = t;
                                }
                            }
                            if (!(*(float *)0x70003194 <= *(float *)0x700031A4)) { hiy = *(float *)0x700031A4; } else { loy = *(float *)0x700031A4; }
                            if (!(*(float *)0x70003198 <= *(float *)0x700031A8)) { hiz = *(float *)0x700031A8; } else { loz = *(float *)0x700031A8; }
                        }
                    }
                }
            }
        }
    }

    return ret;
}
