// NEARMISS func_0019FE50  (vram 0x0019FE50, 0x850 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 93.39% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// NEARMISS: register-allocation web-ORDER permutation in the pass-2 (actor-list) loop. Both available mwcc builds (991202 and 2.3.3) demote the pass-2 outer loop counter's web to the LAST callee-saved register (s6/s7), while the 2.3.1.01 target keeps it in s2 with cnt=s3/p=s4/found=s5/n=s6/list=s7;...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

//
// Segment hit-scan, 2-axis variant (x/z interval clamp + single y probe at
// 0x70003194) of the func_001A0B10 family (see docs/FINDINGS.md "uid state words
// at 0x70003250"). Publishes the query vector base 0x700031D0 = D_700030B0, then:
//   Pass 1: walks the per-uid table at 0x70003250 (count short 0x7000324C, entry
//   stride 4, payload stride 0x28): BREAKS at the first entry with flags bit31
//   clear, skips bit30 entries; stages the area kind byte
//   D_0024D7C0[D_00810700][D_00810701][off28+8] through the scratch short
//   0x70003B88 and gates kinds >= 0x5A plus 0x51 (mode 0x7000324E != 0), 0x52
//   (mode != 2), 0x53 (mode == -1). Hull AABB gate (x lo/hi at hull+0/+0xC, z at
//   +8/+0x14, y probe vs +4/+0x10), then walks the prim list (+0x18 count, +0x1C
//   prims) dispatching on the 0xF000 type nibble: 0x8000/0x4000 ->
//   func_001A4830, 0x2000 -> func_001A4D10, 0x1000 -> func_001A4030 (variable
//   stride advance, 0x800 flag selects the wide form). First hit BREAKS: re-arms
//   the x/z hi bounds from 0x700031B0/0x700031B8, clears the owner 0x700031D4,
//   ORs the kind byte into 0x700030CA's low byte, re-clamps x/z.
//   Pass 2: published actor list D_00275B7C/D_00275B84 (CLASS-4 actors only,
//   skipping the owner at 0x70003254): uid from actor+0xE>>8, same kind gates on
//   actor+0x54, uid bounds check, same AABB + prim walk; hits are processed
//   INSIDE the prim loop (no break), staging the actor pointer at 0x700031D4.
// Returns 1 if the segment cleared both passes, 0 if anything was hit.

extern int D_700030B0[];
extern unsigned char **D_0024D7C0[];
extern unsigned char D_00810700[];
extern unsigned char D_00810701[];
extern unsigned char **D_00275B7C;
extern short D_00275B84;
extern int func_001A4030(void *p);
extern int func_001A4830(void *p);
extern int func_001A4D10(void *p);

int func_0019FE50(void)
{
    float hix, hiz;
    float lox, loz;
    int ret;
    int i;
    int j;
    unsigned char *cnt;
    unsigned char *p;
    int off4;
    int off28;
    unsigned char *uid_table;
    unsigned char *a;
    int found;
    int n;
    unsigned char **list;

    *(int **)0x700031D0 = D_700030B0;
    ret = 1;

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
            unsigned char *hull = uid_table + (*(int *)(uid_table + off4 + 4) & 0x3FFFFFFF);

            if (!(lox < *(float *)(hull + 0)) && (hix <= *(float *)(hull + 0xC)) &&
                !(loz < *(float *)(hull + 8)) && (hiz <= *(float *)(hull + 0x14))) {
                float y = *(float *)0x70003194;

                if (!(y < *(float *)(hull + 4)) && (y <= *(float *)(hull + 0x10))) {
                    int hit = 0;

                    cnt = hull + 0x18;
                    p = cnt + 4;
                    for (j = 0; j < *(short *)cnt; j++) {
                        int ptype = *(short *)p & 0xF000;

                        switch (ptype) {
                        case 0x8000:
                            hit = func_001A4830(p);
                            if (*(short *)p & 0x800) {
                                p += 0x24;
                            } else {
                                p += 0x14;
                            }
                            break;
                        case 0x4000:
                            hit = func_001A4830(p);
                            if (*(short *)p & 0x800) {
                                p += 0x2C;
                            } else {
                                p += 0x18;
                            }
                            break;
                        case 0x2000:
                            hit = func_001A4D10(p);
                            p += 0x1C;
                            break;
                        case 0x1000:
                            hit = func_001A4030(p);
                            if (*(short *)p & 0x800) {
                                int c2 = p[2];
                                unsigned char *q = (unsigned char *)((int)p + 0x24);
                                p = q + c2 * 0x30;
                            } else {
                                int c2 = p[2];
                                unsigned char *q = (unsigned char *)((int)p + 0x14);
                                p = q + c2 * 0x18;
                            }
                            break;
                        }
                        if (hit != 0) {
                            break;
                        }
                    }

                    if (hit != 0) {
                        int k;

                        *(float *)0x700031A0 = *(float *)0x700031B0;
                        ret = 0;
                        k = (*(short *)0x700030CA & 0xFF00) |
                            D_0024D7C0[D_00810700[0]][D_00810701[0]][off28 + 8];
                        *(float *)0x700031A8 = *(float *)0x700031B8;
                        *(void **)0x700031D4 = 0;
                        *(short *)0x700030CA = (short)k;
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

    list = D_00275B7C;

    {
        for (j = 0; j < D_00275B84; j++) {
            a = *list;
            list += 1;
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
                int uid = ((int)*(unsigned short *)(a + 0xE) >> 8) & 0xFF;

                if (uid == 0xFF) {
                    continue;
                }
                {
                    unsigned char *base2 = *(unsigned char **)0x70003250;
                    int *ep = (int *)(base2 + uid * 4 + 4);
                    short kind;

                    if (*ep == 0) {
                        continue;
                    }
                    *(short *)0x70003B88 = a[0x54];
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
                    if (!(uid < *(short *)0x7000324C)) {
                        continue;
                    }
                    if (uid < 0) {
                        continue;
                    }
                    {
                        unsigned char *hull = base2 + *ep;

                        if (!(lox < *(float *)(hull + 0)) && (hix <= *(float *)(hull + 0xC)) &&
                            !(loz < *(float *)(hull + 8)) && (hiz <= *(float *)(hull + 0x14))) {
                            float y = *(float *)0x70003194;

                            if (!(y < *(float *)(hull + 4)) && (y <= *(float *)(hull + 0x10))) {
                                int hit = 0;

                                found = 0;

                                cnt = hull + 0x18;
                                p = cnt + 4;
                                for (n = 0; n < *(short *)cnt; n++) {
                                    int ptype = *(short *)p & 0xF000;

                                    switch (ptype) {
                                    case 0x8000:
                                        hit = func_001A4830(p);
                                        if (*(short *)p & 0x800) {
                                            p += 0x24;
                                        } else {
                                            p += 0x14;
                                        }
                                        break;
                                    case 0x4000:
                                        hit = func_001A4830(p);
                                        if (*(short *)p & 0x800) {
                                            p += 0x2C;
                                        } else {
                                            p += 0x18;
                                        }
                                        break;
                                    case 0x2000:
                                        hit = func_001A4D10(p);
                                        p += 0x1C;
                                        break;
                                    case 0x1000:
                                        hit = func_001A4030(p);
                                        if (*(short *)p & 0x800) {
                                            int c2 = p[2];
                                            unsigned char *q = (unsigned char *)((int)p + 0x24);
                                            p = q + c2 * 0x30;
                                        } else {
                                            int c2 = p[2];
                                            unsigned char *q = (unsigned char *)((int)p + 0x14);
                                            p = q + c2 * 0x18;
                                        }
                                        break;
                                    }
                                    if (hit != 0) {
                                        int k;

                                        found = 1;
                                        k = (*(short *)0x700030CA & 0xFF00) | a[0x54];
                                        ret = 0;
                                        *(float *)0x700031A0 = *(float *)0x700031B0;
                                        *(float *)0x700031A8 = *(float *)0x700031B8;
                                        *(unsigned char **)0x700031D4 = a;
                                        *(short *)0x700030CA = (short)k;
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
    }

    return ret;
}
