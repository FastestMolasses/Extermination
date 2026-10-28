// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
// SPAD: 0x70003250 0x7000324C 0x700030CA

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 63.48% NEARMISS).
// Fixes read from the original instructions (the old body was wrong in places):
// - The original is the twin of func_001A32C0 except for the kind gates, so it is
//   written in that sibling's spelling: relocated scratchpad externs for the uid
//   table 0x70003250, its count 0x7000324C and the kind word 0x700030CA;
//   -sdatathreshold 8 (D_00275B7C/D_00275B84 gp-relative); function-scope
//   counters/cursors; `a = *list++` walk.
// - Pass 1 indexes the uid table and placement records by stepped offsets
//   (+4 / +0x28) and BREAKS at the first entry with flags bit31 clear (the old C
//   continued); it keeps the entry pointer and re-reads the word for the hull.
// - The pass-1 placement kind gate keeps kinds in [0x1E, 0x5A). Pass 2 has NO actor
//   kind gate.
// - AABB test order is y, x, z; x/z are read straight from scratchpad.
// - Prim dispatch has no default. Pass 1: 0x8000 only advances. Pass 2: 0x8000
//   ALSO calls func_001A44B0 (advance 0x24/0x14), as does 0x4000 (0x2C/0x18).
// - The hit blocks store the floor-probe result 0x700031B4 into the HIGH y bound
//   word 0x700031A4, then stage 0x700031D4 (0 / the actor), OR the kind byte into
//   0x700030CA and re-clamp the y interval from 0x70003194/A4.
//
// SEMANTICS: movable-object hull segment query, sibling of func_001A32C0/
// func_001A0B10/func_001A2AE0/func_001A1B80 (docs/FINDINGS.md "4. Movable-object
// hulls" and "The uid state words at 0x70003250"). Primes the hit-normal record
// pointer 0x700031D0 to D_700030B0 and clamps the query y interval (0x70003194 /
// 0x700031A4) into lo/hi; x 0x70003190 and z 0x70003198 are point values.
//   Pass 1: per-uid table at 0x70003250 (count 0x7000324C); breaks at the first
//   entry with bit31 clear, skips bit30 entries and bit29 entries while the mode
//   short 0x7000324E is 0, and keeps only placement kinds
//   (D_0024D7C0[D_00810700][D_00810701][uid*0x28+8]) in [0x1E, 0x5A). Hull AABB
//   test (y range, then x, then z points); on pass, walks the prim list (+0x18
//   count, +0x1C prims) dispatching on the 0xF000 type nibble to func_001A44B0
//   (0x4000) / func_001A4650 (0x2000) / func_001A4030 (0x1000); 0x8000 only
//   advances. On the first hit: copy 0x700031B4 into 0x700031A4, clear the hit
//   owner 0x700031D4, OR the kind byte into 0x700030CA's low byte, re-clamp y.
//   Pass 2: published CLASS-4 actor list (D_00275B7C/D_00275B84); skips the actor
//   at 0x70003254, same uid lookup + AABB (no kind gate); same dispatch except
//   0x8000 also calls func_001A44B0. On the first hit: same as pass 1 but stages
//   the actor pointer at 0x700031D4 and ORs the actor kind (+0x54).
// Returns 1 if no hit was found in either pass, 0 otherwise.

typedef struct Adv48 { int w[12]; } Adv48;   /* 0x30-byte tri record */
typedef struct Adv24 { int w[6]; } Adv24;    /* 0x18-byte tri record */

extern int D_700030B0[];
extern void *D_700031D4[];
extern unsigned char *D_70003250[];
extern short D_7000324C[];
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

int func_001A3980(void)
{
    float hiy;
    float loy;
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
    ret = 1;

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
        {
            int kind = D_0024D7C0[D_00810700[0]][D_00810701[0]][off28 + 8];
            if (kind < 0x1E) {
                continue;
            }
            if (!(kind < 0x5A)) {
                continue;
            }
        }
        {
            unsigned char *hull = uid_table + (*fp & 0x3FFFFFFF);

            if (!(loy < *(float *)(hull + 4)) && (hiy <= *(float *)(hull + 0x10)) &&
                !(*(float *)0x70003190 < *(float *)(hull + 0)) && (*(float *)0x70003190 <= *(float *)(hull + 0xC)) &&
                !(*(float *)0x70003198 < *(float *)(hull + 8)) && (*(float *)0x70003198 <= *(float *)(hull + 0x14))) {
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
                    int kb;
                    int v;
                    float lo;
                    float t;

                    D_700031A4[0] = *(float *)0x700031B4;
                    D_700031D4[0] = 0;
                    kb = D_0024D7C0[D_00810700[0]][D_00810701[0]][off28 + 8];
                    v = D_700030CA[0];
                    ret = 0;
                    lo = *(float *)0x70003194;
                    t = *(float *)0x700031A4;

                    D_700030CA[0] = (short)((v & 0xFF00) | kb);
                    if (!(lo <= t)) {
                        hiy = t;
                    } else {
                        loy = t;
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
                if (!(uid < D_7000324C[0])) {
                    continue;
                }
                if (uid < 0) {
                    continue;
                }
                {
                    unsigned char *hull = tbl + off;

                    if (!(loy < *(float *)(hull + 4)) && (hiy <= *(float *)(hull + 0x10)) &&
                        !(*(float *)0x70003190 < *(float *)(hull + 0)) && (*(float *)0x70003190 <= *(float *)(hull + 0xC)) &&
                        !(*(float *)0x70003198 < *(float *)(hull + 8)) && (*(float *)0x70003198 <= *(float *)(hull + 0x14))) {
                        cnt2 = hull + 0x18;
                        p2 = cnt2 + 4;
                        hit = 0;
                        for (k = 0; k < *(short *)cnt2; k++) {
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
                                break;
                            }
                        }

                        if (hit != 0) {
                            int kb;
                            int v;
                            float lo;
                            float t;

                            D_700031A4[0] = *(float *)0x700031B4;
                            D_700031D4[0] = a;
                            kb = a[0x54];
                            v = D_700030CA[0];
                            ret = 0;
                            lo = *(float *)0x70003194;
                            t = *(float *)0x700031A4;

                            D_700030CA[0] = (short)((v & 0xFF00) | kb);
                            if (!(lo <= t)) {
                                hiy = t;
                            } else {
                                loy = t;
                            }
                        }
                    }
                }
            }
        }
    }

    return ret;
}
