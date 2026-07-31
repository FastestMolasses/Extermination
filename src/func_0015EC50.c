// NEARMISS func_0015EC50  (vram 0x0015EC50, 0x11A0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 96.50% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Same two artifacts as anim_matrix_multi; 1080/1128 instructions byte-identical. All logic recovered and cross-checked instruction-by-instruction: the 6 level/sub-level exclusion-box chains (including the nested shared-Y test that only level 0xF uses), the forward/side probe combination and the descen...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// "Can this actor step down / drop off the ledge in front of it?" test.
// Returns 1 when the actor should switch to the drop-down state (e[5] = 6,
// sub-state 0, anim 0xC), 0 otherwise.
//
// 1. Bail out if the actor is already flagged (e+0x314), or if its current
//    attachment (e+0x308) is of kind 0x28, or of kind 2 while the actor is
//    facing more than +/-90 degrees away.
// 2. Per-level hand-authored exclusion boxes: for a handful of (level,
//    sub-level) pairs a set of world-space AABBs around the actor position
//    (e+0xB0/0xB4/0xB8) suppress the ledge probe entirely.
// 3. Otherwise cast a forward probe (scaled by the actor size class at
//    e+0x25C), then two side probes; the combination of forward hit and side
//    blockers decides whether there is a real ledge. Finally the drop target
//    is validated against the camera/target record at 0x700031D0 and the
//    height table at 0x700030F0 / flag table at 0x70003170.

extern unsigned char D_00810700;
extern unsigned char D_00810701;
extern float D_002489E0[];
extern float D_700030F0[];
extern unsigned short D_70003170[];
extern float D_700036A0[];
extern float D_700038A0[];
extern float D_700038B0[];
extern float D_700038C0[];
extern float D_700038D0[];

extern void func_00102948(float *dst, float *src);
extern void func_001026A0(float *dst, float *m, float *v);
extern void func_00102918(float *dst, float *m, float *v);
extern void func_001029C0(float *m);
extern void func_00102BB0(float *dst, float *m, float yaw);
extern float func_0011E748(float x);
extern void func_00177510(void);
extern int func_0019AD00(void *e, float *v, int mask);
extern int func_0019AFE0(void *e, float *a, float *b, int mask);
extern void func_0019BC40(float *v);

int func_0015EC50(unsigned char *e) {
    float pos[4];
    float dist;
    float probe;
    float dz;
    float px;
    float pz;
    int hit;
    int side;
    int lvl;
    int i;
    unsigned char *att;
    unsigned char *tgt;

    if (e[0x314] == 1) {
        return 0;
    }
    att = *(unsigned char **)(e + 0x308);
    if (att != 0) {
        if (att[3] == 0x28) {
            return 0;
        }
        if (att[3] == 2) {
            if (!(*(float *)(e + 0xC4) < 1.5707964f) ||
                *(float *)(e + 0xC4) <= -1.5707964f) {
                return 0;
            }
        }
    }
    lvl = D_00810700;
    if (lvl == 4) {
        if (*(float *)(e + 0xB4) >= 60.0f && *(float *)(e + 0xB4) <= 70.0f &&
            *(float *)(e + 0xB0) >= 410.0f && *(float *)(e + 0xB0) <= 465.0f &&
            *(float *)(e + 0xB8) >= 235.0f && *(float *)(e + 0xB8) <= 285.0f) {
            return 0;
        }
    } else if (lvl == 0xD && D_00810701 == 0) {
        if (*(float *)(e + 0xB4) >= 205.0f && *(float *)(e + 0xB4) <= 220.0f &&
            *(float *)(e + 0xB0) >= 670.0f && *(float *)(e + 0xB0) <= 740.0f &&
            *(float *)(e + 0xB8) >= 1215.0f && *(float *)(e + 0xB8) <= 1290.0f) {
            return 0;
        }
        if (*(float *)(e + 0xB4) >= 150.0f && *(float *)(e + 0xB4) <= 210.0f &&
            *(float *)(e + 0xB0) >= 720.0f && *(float *)(e + 0xB0) <= 800.0f &&
            *(float *)(e + 0xB8) >= 800.0f && *(float *)(e + 0xB8) <= 840.0f) {
            return 0;
        }
        if (*(float *)(e + 0xB4) >= 150.0f && *(float *)(e + 0xB4) <= 215.0f &&
            *(float *)(e + 0xB0) >= 635.0f && *(float *)(e + 0xB0) <= 720.0f &&
            *(float *)(e + 0xB8) >= 1270.0f && *(float *)(e + 0xB8) <= 1325.0f) {
            return 0;
        }
    } else if (lvl == 0xF && D_00810701 == 1) {
        if (*(float *)(e + 0xB4) >= 285.0f && *(float *)(e + 0xB4) <= 345.0f) {
            if (*(float *)(e + 0xB0) >= 835.0f && *(float *)(e + 0xB0) <= 910.0f &&
                *(float *)(e + 0xB8) >= 820.0f && *(float *)(e + 0xB8) <= 850.0f) {
                return 0;
            }
            if (*(float *)(e + 0xB0) >= 885.0f && *(float *)(e + 0xB0) <= 910.0f &&
                *(float *)(e + 0xB8) >= 830.0f && *(float *)(e + 0xB8) <= 905.0f) {
                return 0;
            }
        }
        if (*(float *)(e + 0xB4) >= 310.0f && *(float *)(e + 0xB4) <= 335.0f &&
            *(float *)(e + 0xB0) >= 865.0f && *(float *)(e + 0xB0) <= 890.0f &&
            *(float *)(e + 0xB8) >= 930.0f && *(float *)(e + 0xB8) <= 980.0f) {
            return 0;
        }
    } else if (lvl == 0x10 && D_00810701 == 1) {
        if (*(float *)(e + 0xB4) >= 95.0f && *(float *)(e + 0xB4) <= 205.0f &&
            *(float *)(e + 0xB0) >= 170.0f && *(float *)(e + 0xB0) <= 230.0f &&
            *(float *)(e + 0xB8) >= 90.0f && *(float *)(e + 0xB8) <= 260.0f) {
            return 0;
        }
        if (*(float *)(e + 0xB4) >= 99.0f && *(float *)(e + 0xB4) <= 106.0f &&
            *(float *)(e + 0xB0) >= 228.0f && *(float *)(e + 0xB0) <= 258.0f &&
            *(float *)(e + 0xB8) >= 181.0f && *(float *)(e + 0xB8) <= 234.0f) {
            return 0;
        }
    } else if (lvl == 0x13 && D_00810701 == 0) {
        if (*(float *)(e + 0xB4) >= 260.0f && *(float *)(e + 0xB4) <= 270.0f &&
            *(float *)(e + 0xB0) >= 1000.0f && *(float *)(e + 0xB0) <= 1035.0f &&
            *(float *)(e + 0xB8) >= 990.0f && *(float *)(e + 0xB8) <= 1130.0f) {
            return 0;
        }
        if (*(float *)(e + 0xB4) >= 210.0f && *(float *)(e + 0xB4) <= 230.0f &&
            *(float *)(e + 0xB0) >= 1040.0f && *(float *)(e + 0xB0) <= 1100.0f &&
            *(float *)(e + 0xB8) >= 970.0f && *(float *)(e + 0xB8) <= 1050.0f) {
            return 0;
        }
        if (*(float *)(e + 0xB4) >= 185.0f && *(float *)(e + 0xB4) <= 210.0f &&
            *(float *)(e + 0xB0) >= 985.0f && *(float *)(e + 0xB0) <= 1020.0f &&
            *(float *)(e + 0xB8) >= 920.0f && *(float *)(e + 0xB8) <= 950.0f) {
            return 0;
        }
    } else if (lvl == 0x16) {
        if (*(float *)(e + 0xB4) >= 230.0f && *(float *)(e + 0xB4) <= 250.0f &&
            *(float *)(e + 0xB0) >= 95.0f && *(float *)(e + 0xB0) <= 135.0f &&
            *(float *)(e + 0xB8) >= 235.0f && *(float *)(e + 0xB8) <= 285.0f) {
            return 0;
        }
    }

    if (e[0x25C] < 2) {
        probe = 2.0f;
    } else if (e[0x25C] == 2) {
        probe = 3.0f;
    } else {
        probe = 4.0f;
    }
    func_00102948(D_700038A0, D_002489E0);
    *(float *)0x700038A8 *= probe;
    func_001026A0(D_700038B0, (float *)(e + 0xD0), D_700038A0);
    *(float *)0x700038B4 += 1.0f;
    if (func_0019AD00(e, D_700038B0, 7) != 0) {
        func_00177510();
        *(float *)0x70003A20 = *(float *)(e + 0xB0) - *(float *)0x70003050;
        dz = *(float *)(e + 0xB8) - *(float *)0x70003058;
        *(float *)0x70003A28 = dz;
        dist = func_0011E748(*(float *)0x70003A20 * *(float *)0x70003A20 + dz * dz);
        hit = 1;
    } else {
        *(float *)0x700038B4 += 10.0f;
        if (func_0019AD00(e, D_700038B0, 7) != 0) {
            func_00177510();
            *(float *)0x70003A20 = *(float *)(e + 0xB0) - *(float *)0x70003050;
            dz = *(float *)(e + 0xB8) - *(float *)0x70003058;
            *(float *)0x70003A28 = dz;
            dist = func_0011E748(*(float *)0x70003A20 * *(float *)0x70003A20 + dz * dz);
            hit = 1;
        } else {
            hit = 0;
        }
    }
    if (hit != 0) {
        px = *(float *)0x70003050;
        pz = *(float *)0x70003058;
        pos[0] = px;
        pos[2] = pz;
    } else {
        px = *(float *)0x700038B0;
        pz = *(float *)0x700038B8;
        pos[0] = px;
        pos[2] = pz;
    }
    pos[1] = *(float *)(e + 0xB4);
    side = 0;
    func_001029C0(D_700036A0);
    func_00102BB0(D_700036A0, D_700036A0, *(float *)(e + 0xC4));
    func_00102918(D_700036A0, D_700036A0, pos);
    *(float *)0x700038A0 = -0.1f;
    *(float *)0x700038A4 = 10.0f;
    *(int *)0x700038A8 = 0;
    *(float *)0x700038AC = 1.0f;
    *(float *)0x700038B0 = -5.0f;
    *(float *)0x700038B4 = 10.0f;
    *(int *)0x700038B8 = 0;
    *(float *)0x700038BC = 1.0f;
    func_001026A0(D_700038C0, D_700036A0, D_700038A0);
    func_001026A0(D_700038D0, D_700036A0, D_700038B0);
    if (func_0019AFE0(e, D_700038C0, D_700038D0, 6) != 0) {
        side = 1;
    }
    *(float *)0x700038A0 = 0.1f;
    *(float *)0x700038A4 = 10.0f;
    *(int *)0x700038A8 = 0;
    *(float *)0x700038AC = 1.0f;
    *(float *)0x700038B0 = 5.0f;
    *(float *)0x700038B4 = 10.0f;
    *(int *)0x700038B8 = 0;
    *(float *)0x700038BC = 1.0f;
    func_001026A0(D_700038C0, D_700036A0, D_700038A0);
    func_001026A0(D_700038D0, D_700036A0, D_700038B0);
    if (func_0019AFE0(e, D_700038C0, D_700038D0, 6) != 0) {
        side |= 2;
    }
    if (hit == 0) {
        if (side == 3) {
            return 0;
        }
    } else if (side != 0) {
        return 0;
    }
    pos[1] = *(float *)(e + 0xB4) - 1.0f;
    if (e[0x319] & 1) {
        func_00102948(D_700038D0, (float *)(e + 0xB0));
        *(float *)0x700038D4 -= 1.0f;
    } else {
        *(int *)0x700038A0 = 0;
        *(float *)0x700038A4 = -1.0f;
        *(float *)0x700038A8 = -2.0f;
        *(float *)0x700038AC = 1.0f;
        func_001026A0(D_700038D0, (float *)(e + 0xD0), D_700038A0);
    }
    if (func_0019AFE0(e, pos, D_700038D0, 6) != 0) {
        if ((*(short *)(*(int *)0x700031D0 + 0x1A) & 0xFF00) == 0x2000) {
            if (hit != 0) {
                *(float *)0x70003A20 = *(float *)(e + 0xB0) - *(float *)0x700031B0;
                dz = *(float *)(e + 0xB8) - *(float *)0x700031B8;
                *(float *)0x70003A28 = dz;
                *(float *)0x70003A2C =
                    func_0011E748(*(float *)0x70003A20 * *(float *)0x70003A20 + dz * dz);
                if (dist <= 13.5f + *(float *)0x70003A2C) {
                    return 0;
                }
            }
            tgt = *(unsigned char **)0x700031D0;
            *(float *)0x700038B0 = *(float *)0x700031B0 + 4.5f * *(float *)(tgt + 0x24);
            *(float *)0x700038B8 = *(float *)0x700031B8 + 4.5f * *(float *)(tgt + 0x2C);
            *(float *)0x700038B4 = *(float *)(e + 0xB4);
            func_0019BC40(D_700038B0);
            if (*(int *)0x700031E0 != 0) {
                for (i = *(int *)0x700031E0 - 1; i >= 0; i--) {
                    if (D_70003170[i] & 1) {
                        if (D_700030F0[i] <= 4.01f + *(float *)(e + 0xB4)) {
                            *(float *)(e + 0x254) = D_700030F0[i] - *(float *)(e + 0xB4);
                            if (*(float *)(e + 0x254) < -4.01f) {
                                e[5] = 6;
                                e[6] = 0;
                                e[0x1F0] = 0xC;
                                return 1;
                            }
                            return 0;
                        }
                    }
                }
            } else {
                e[5] = 6;
                e[6] = 0;
                e[0x1F0] = 0xC;
                return 1;
            }
        }
    }
    return 0;
}
