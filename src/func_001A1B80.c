// NEARMISS func_001A1B80  (vram 0x001A1B80, 0x7F0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 76.03% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Genuine register-coloring + branch-likely-sense scheduling artifact. Full logic/structure recovered (correct 8-saved-GPR frame including the genuinely-needed extra s7 for pass-2's local hit-flag, exact f20-f25 clamp coloring, exact switch dispatch order via reversed case labels). Sole residual is...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

// SEMANTICS: segment hit-scan sibling of func_001A2AE0/func_001A32C0/
// func_001A3980/func_001A06A0 (see docs/FINDINGS.md "4. The uid state
// words at 0x70003250"). Clamps the query interval on all three axes
// (x/y/z, D_70003190.98 lo / D_700031A0.A8 hi), then:
//   Pass 1: per-uid table walk at 0x70003250 (count 0x7000324C), gated
//   live/enabled (flags bit31 set, bits 30|29 both clear -- a single
//   combined 0x60000000 mask test, no D_7000324E gate here) and
//   area-descriptor kind (+8, D_0024D7C0[area][sub] ==
//   D_00810700/D_00810701, records 0x28 bytes, index = uid) < 0x5A.
//   Hull AABB test against all 3 axes; on pass, walks the prim list
//   (+0x18 count, +0x1C prims) dispatching by the 0xF000 type nibble to
//   func_001A5C30 (0x4000/0x8000) / func_001A50A0 (0x2000) /
//   func_001A4030 (0x1000). On any nonzero hit: clamp y to the
//   floor-probe result 0x700031B4, clear hit-owner 0x700031D4, OR the
//   placement kind byte into 0x700030CA low byte, re-clamp all 3 axes.
//   Pass 2: published CLASS-4 actor list (D_00275B7C/D_00275B84); same
//   uid lookup + AABB + prim dispatch (same 3 callees), kind gate direct
//   at actor +0x54 < 0x5A. On hit (checked per-prim, inside the dispatch
//   loop): re-splats D_70003190[0..2].+0x10 from +0x20 (a 3-word vector
//   fixup), stages the hit actor pointer at 0x700031D4, ORs actor kind
//   byte into 0x700030CA, sets a local hit-flag. After the prim loop,
//   the 3-axis re-clamp is deferred and runs once guarded by that flag.
// Returns 1 if either pass recorded a hit, 0 if the segment cleared both.

extern int D_700030B0;
extern unsigned char **D_0024D7C0[];
extern unsigned char D_00810700;
extern unsigned char D_00810701;
extern unsigned char **D_00275B7C;
extern short D_00275B84;
extern int func_001A4030(void *p);
extern int func_001A50A0(void *p);
extern int func_001A5C30(void *p);

int func_001A1B80(void)
{
    float hix, hiy, hiz;
    float lox, loy, loz;
    int ret;
    int i;
    unsigned char *uid_table;

    *(int **)0x700031D0 = &D_700030B0;

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
        hiy = loy;
        if (!(loy <= t)) {
            hiy = t;
        } else {
            loy = t;
        }
    }

    loz = *(float *)0x70003198;
    {
        float t = *(float *)0x700031A8;
        hiz = loz;
        if (!(loz <= t)) {
            hiz = t;
        } else {
            loz = t;
        }
    }

    ret = 0;

    for (i = 0; i < *(short *)0x7000324C; i++) {
        int off;

        uid_table = *(unsigned char **)0x70003250;
        off = *(int *)(uid_table + i * 4 + 4);

        if (!(off & 0x80000000)) {
            continue;
        }
        if (off & 0x60000000) {
            continue;
        }

        if ((*(unsigned char **)((char *)D_0024D7C0[D_00810700] + D_00810701 * 4))[i * 0x28 + 8] >= 0x5A) {
            continue;
        }
        {
            unsigned char *hull = uid_table + (off & 0x3FFFFFFF);

            if (!(lox < *(float *)(hull + 0)) && (hix <= *(float *)(hull + 0xC)) &&
                !(loy < *(float *)(hull + 4)) && (hiy <= *(float *)(hull + 0x10)) &&
                !(loz < *(float *)(hull + 8)) && (hiz <= *(float *)(hull + 0x14))) {
                unsigned char *p = hull + 0x18 + 4;
                int hit = 0;
                int j;

                for (j = 0; j < *(short *)(hull + 0x18); j++) {
                    int ptype = *(short *)p & 0xF000;

                    switch (ptype) {
                    default:
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
                            p = p + 0x24 + p[2] * 0x30;
                        } else {
                            p = p + 0x14 + p[2] * 0x18;
                        }
                        break;
                    }
                    if (hit != 0) {
                        break;
                    }
                }

                if (hit != 0) {
                    ret = 1;
                    loy = *(float *)0x700031B4;
                    *(void **)0x700031D4 = 0;
                    *(short *)0x700030CA = (short)((*(short *)0x700030CA & 0xFF00) |
                        (*(unsigned char **)((char *)D_0024D7C0[D_00810700] + D_00810701 * 4))[i * 0x28 + 8]);
                    {
                        float t = *(float *)0x700031A0;
                        if (!(*(float *)0x70003190 <= t)) {
                            hix = t;
                        } else {
                            lox = t;
                        }
                    }
                    {
                        float t = *(float *)0x700031A4;
                        if (!(*(float *)0x70003194 <= t)) {
                            hiy = t;
                        } else {
                            loy = t;
                        }
                    }
                    {
                        float t = *(float *)0x700031A8;
                        if (!(*(float *)0x70003198 <= t)) {
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

        for (i = 0; i < D_00275B84; i++, list++) {
            unsigned char *a = *list;

            if (a == 0 || (a[2] & 0x1F) != 4 || *(unsigned char **)0x70003254 == a) {
                continue;
            }
            {
                int uid = (*(unsigned short *)(a + 0xE) >> 8) & 0xFF;

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
                            !(loy < *(float *)(hull + 4)) && (hiy <= *(float *)(hull + 0x10)) &&
                            !(loz < *(float *)(hull + 8)) && (hiz <= *(float *)(hull + 0x14)) &&
                            (a[0x54] < 0x5A)) {
                            unsigned char *p = hull + 0x18 + 4;
                            int hit = 0;
                            int hit_flag = 0;
                            int j;

                            for (j = 0; j < *(short *)(hull + 0x18); j++) {
                                int ptype = *(short *)p & 0xF000;

                                switch (ptype) {
                                default:
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
                                        p = p + 0x24 + p[2] * 0x30;
                                    } else {
                                        p = p + 0x14 + p[2] * 0x18;
                                    }
                                    break;
                                }

                                if (hit != 0) {
                                    {
                                        float *v = (float *)0x70003190;
                                        int k = 0;
                                        do {
                                            k += 1;
                                            v[4] = v[8];
                                            v += 1;
                                        } while (k < 3);
                                    }
                                    *(unsigned char **)0x700031D4 = a;
                                    ret = 1;
                                    *(short *)0x700030CA = (short)((*(short *)0x700030CA & 0xFF00) | a[0x54]);
                                    hit_flag = 1;
                                }
                            }

                            if (hit_flag != 0) {
                                {
                                    float t = *(float *)0x700031A0;
                                    if (!(*(float *)0x70003190 <= t)) {
                                        hix = t;
                                    } else {
                                        lox = t;
                                    }
                                }
                                {
                                    float t = *(float *)0x700031A4;
                                    if (!(*(float *)0x70003194 <= t)) {
                                        hiy = t;
                                    } else {
                                        loy = t;
                                    }
                                }
                                {
                                    float t = *(float *)0x700031A8;
                                    if (!(*(float *)0x70003198 <= t)) {
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
