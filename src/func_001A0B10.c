// NEARMISS func_001A0B10  (vram 0x001A0B10, 0x878 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 95.37% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation web permutation + list-scheduling residual (NEARMISS harvest; body and structure fully recovered; up from the sibling func_001A2AE0's 77.20% park on the same class): (1) pass-2 saved-reg webs rotated — target keeps function-scope j/cnt/p in s2/s3/s4 across BOTH passes and recy...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

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

extern int D_700030B0[];
extern float D_70003190[];
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
    unsigned char *uid_table;

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

    for (i = 0, off4 = 0, off28 = 0; i < *(short *)0x7000324C; off4 += 4, off28 += 0x28, i++) {
        int off;
        short kind;

        uid_table = *(unsigned char **)0x70003250;
        off = *(int *)(uid_table + off4 + 4);

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
            unsigned char *hull = uid_table + (*(int *)(uid_table + off4 + 4) & 0x3FFFFFFF);

            if (!(lox < *(float *)(hull + 0)) && (hix <= *(float *)(hull + 0xC)) &&
                !(loz < *(float *)(hull + 8)) && (hiz <= *(float *)(hull + 0x14)) &&
                !(loy < *(float *)(hull + 4)) && (hiy <= *(float *)(hull + 0x10))) {
                int hit = 0;

                cnt = hull + 0x18;
                p = cnt + 4;
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
                            unsigned char *q = (unsigned char *)((int)p + 0x24);
                            int c2 = p[2];
                            p = q + c2 * 0x30;
                        } else {
                            unsigned char *q = (unsigned char *)((int)p + 0x14);
                            int c2 = p[2];
                            p = q + c2 * 0x18;
                        }
                        break;
                    }
                    if (hit != 0) {
                        break;
                    }
                }

                if (hit != 0) {
                    {
                        int k = 0;
                        float *v = D_70003190;
                        do {
                            k += 1;
                            v[4] = v[8];
                            v += 1;
                        } while (k < 3);
                    }
                    *(void **)0x700031D4 = 0;
                    ret = 1;
                    *(short *)0x700030CA = (short)((*(short *)0x700030CA & 0xFF00) |
                        D_0024D7C0[D_00810700[0]][D_00810701[0]][off28 + 8]);
                    {
                        float lo = *(float *)0x70003190;
                        float t = *(float *)0x700031A0;
                        if (!(lo <= t)) {
                            hix = t;
                        } else {
                            lox = t;
                        }
                    }
                    {
                        float lo = *(float *)0x70003194;
                        float t = *(float *)0x700031A4;
                        if (!(lo <= t)) {
                            hiy = t;
                        } else {
                            loy = t;
                        }
                    }
                    {
                        float lo = *(float *)0x70003198;
                        float t = *(float *)0x700031A8;
                        if (!(lo <= t)) {
                            hiz = t;
                        } else {
                            loz = t;
                        }
                    }
                }
            }
        }
    }

    {
        unsigned char **list = D_00275B7C;

        for (j = 0; j < D_00275B84; list++, j++) {
            unsigned char *a = *list;

            if (a[0] == 0 || (a[2] & 0x1F) != 4 || *(unsigned char **)0x70003254 == a) {
                continue;
            }
            {
                int uid = ((int)*(unsigned short *)(a + 0xE) >> 8) & 0xFF;

                if (uid == 0xFF) {
                    continue;
                }
                {
                    unsigned char *base2 = *(unsigned char **)0x70003250;
                    int off2 = *(int *)(base2 + uid * 4 + 4);

                    if (off2 == 0) {
                        continue;
                    }
                    if (!(uid < *(short *)0x7000324C)) {
                        continue;
                    }
                    if (uid < 0) {
                        continue;
                    }
                    {
                        unsigned char *hull = base2 + off2;

                        if (!(lox < *(float *)(hull + 0)) && (hix <= *(float *)(hull + 0xC)) &&
                            !(loz < *(float *)(hull + 8)) && (hiz <= *(float *)(hull + 0x14)) &&
                            !(loy < *(float *)(hull + 4)) && (hiy <= *(float *)(hull + 0x10)) &&
                            (a[0x54] < 0x50)) {
                            int found = 0;
                            int hit = 0;
                            int n;

                            cnt = hull + 0x18;
                            p = cnt + 4;
                            for (n = 0; n < *(short *)cnt; n++) {
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
                                        unsigned char *q = (unsigned char *)((int)p + 0x24);
                                        int c2 = p[2];
                                        p = q + c2 * 0x30;
                                    } else {
                                        unsigned char *q = (unsigned char *)((int)p + 0x14);
                                        int c2 = p[2];
                                        p = q + c2 * 0x18;
                                    }
                                    break;
                                }
                                if (hit != 0) {
                                    found = 1;
                                    {
                                        int k = 0;
                                        float *v = D_70003190;
                                        do {
                                            k += 1;
                                            v[4] = v[8];
                                            v += 1;
                                        } while (k < 3);
                                    }
                                    *(unsigned char **)0x700031D4 = a;
                                    ret = 1;
                                    *(short *)0x700030CA = (short)((*(short *)0x700030CA & 0xFF00) | a[0x54]);
                                }
                            }

                            if (found != 0) {
                                {
                                    float lo = *(float *)0x70003190;
                                    float t = *(float *)0x700031A0;
                                    if (!(lo <= t)) {
                                        hix = t;
                                    } else {
                                        lox = t;
                                    }
                                }
                                {
                                    float lo = *(float *)0x70003194;
                                    float t = *(float *)0x700031A4;
                                    if (!(lo <= t)) {
                                        hiy = t;
                                    } else {
                                        loy = t;
                                    }
                                }
                                {
                                    float lo = *(float *)0x70003198;
                                    float t = *(float *)0x700031A8;
                                    if (!(lo <= t)) {
                                        hiz = t;
                                    } else {
                                        loz = t;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return ret;
}
