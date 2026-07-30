// NEARMISS func_001A50A0  (vram 0x001A50A0, 0x600 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 95.33% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// 69 of 387 instructions differ; the instruction SEQUENCE is aligned 1:1 with the target everywhere except one reload, so the decode is verified faithful. Residuals, all in the post-loop switch plus 3 in the loop tail: (1) LOOP-TAIL SCHEDULING (3 instrs): the three pointer bumps around the `swc1 pr...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// SEMANTICS: segment-vs-AABB single-face intersection test on the scratchpad,
// dispatched on the requested face code at arg0+2 through jtbl_0026DA80
// (7 entries; face 0 and any code >= 7 return 0 immediately).
// Inputs:  D_70003190[0..2] = segment start A, D_700031A0[0..2] (= D_70003190[4..6]) = end B;
//          arg0+4..0xC = box origin xyz, arg0+0x10..0x18 = box extents xyz (may be negative).
// Pass 1 (the 3-axis loop) writes the working set into the scratchpad:
//   D_70003620[i] = B[i] - A[i]                       (segment delta)
//   D_70003600[i] / D_70003610[i] = axis min / max    (extent sign normalised)
//   D_70003630[i] = boxorigin[i] - A[i]
//   and rejects up front when the segment travels away from the requested face
//   (delta < 0 rejects the +face codes 2/4/6, delta >= 0 rejects 1/3/5).
// Pass 2 (the switch) solves the plane crossing on the tested axis:
//   requires D_70003630[axis] * (D_70003600[axis] - B[axis]) < 0,
//   t = D_70003630[axis] / D_70003620[axis]           -> 0x70003680
//   and the two cross-axis coordinates A[j] + delta[j]*t -> 0x70003684 / 0x70003688,
//   both of which must lie strictly inside that axis' (min, max).
// On a hit it publishes the contact point at 0x700031B0..0x700031B8 (the tested
// axis takes the box-plane coordinate straight from arg0+4+4*axis), the face bit
// at 0x700030CA (0x2000 for X/Z faces, 0x4000 / -0x8000 for the Y faces) and the
// unit face normal (+/-1.0f) in 0x700030D4 / 0x700030D8 / 0x700030DC, and returns 1.
// Any miss returns 0.
extern float D_70003190[];
extern float D_70003600[];
extern float D_70003610[];
extern float D_70003620[];
extern float D_70003630[];

int func_001A50A0(char *arg0) {
    int i;
    int face;
    float *box;
    float *pa;
    float *pd;
    float *pbox;
    float *pmin;
    float *pmax;
    float *prel;
    float d;
    float e;

    pa = D_70003190;
    face = *(unsigned char *)(arg0 + 2);
    pd = D_70003620;
    pmin = D_70003600;
    pmax = D_70003610;
    box = (float *)(arg0 + 4);
    prel = D_70003630;
    i = 0;
    pbox = box;
    do {
        d = pa[4] - pa[0];
        *pd = d;
        if (d < 0.0f) {
            if (i == 0) {
                if (face == 2) {
                    return 0;
                }
            } else if (i == 1) {
                if (face == 4) {
                    return 0;
                }
            } else if (i == 2) {
                if (face == 6) {
                    return 0;
                }
            }
        } else {
            if (i == 0) {
                if (face == 1) {
                    return 0;
                }
            } else if (i == 1) {
                if (face == 3) {
                    return 0;
                }
            } else if (i == 2) {
                if (face == 5) {
                    return 0;
                }
            }
        }
        e = pbox[3];
        if (!(e <= 0.0f)) {
            *pmin = pbox[0];
            *pmax = pbox[0] + pbox[3];
        } else {
            *pmin = pbox[0] + e;
            *pmax = pbox[0];
        }
        i += 1;
        pd += 1;
        *prel = pbox[0] - pa[0];
        pmin += 1;
        pbox += 1;
        pa += 1;
        pmax += 1;
        prel += 1;
    } while (i < 3);
    switch (face) {
    case 1:
    case 2:
    {
        float dn;
        float t;
        float ax;
        float bx;
        float by;
        float ay;
        float lo;
        float x;
        float y;
        dn = *(float *)0x70003630;
        if (!(dn * (*(float *)0x70003600 - *(float *)0x700031A0) < 0.0f)) {
            return 0;
        }
        t = dn / *(float *)0x70003620;
        ax = *(float *)0x70003624;
        bx = *(float *)0x70003194;
        by = *(float *)0x70003198;
        ay = *(float *)0x70003628;
        lo = *(float *)0x70003604;
        *(float *)0x70003680 = t;
        *(float *)0x70003684 = bx + (ax * t);
        x = *(volatile float *)0x70003684;
        *(float *)0x70003688 = by + (ay * t);
        if (x > lo) {
            if (x < *(float *)0x70003614) {
                y = *(float *)0x70003688;
                if (y > *(float *)0x70003608) {
                    if (y < *(float *)0x70003618) {
                        *(float *)0x700031B0 = box[0];
                        *(float *)0x700031B4 = x;
                        *(float *)0x700031B8 = y;
                        *(short *)0x700030CA = 0x2000;
                        if (face == 1) {
                            *(int *)0x700030D4 = 0x3F800000;
                        } else {
                            *(int *)0x700030D4 = 0xBF800000;
                        }
                        *(int *)0x700030DC = 0;
                        *(int *)0x700030D8 = 0;
                        return 1;
                    }
                }
            }
        }
        return 0;
    }
    case 3:
    case 4:
    {
        float dn;
        float t;
        float ax;
        float bx;
        float by;
        float ay;
        float lo;
        float x;
        float y;
        dn = *(float *)0x70003634;
        if (!(dn * (*(float *)0x70003604 - *(float *)0x700031A4) < 0.0f)) {
            return 0;
        }
        t = dn / *(float *)0x70003624;
        ax = *(float *)0x70003620;
        bx = *(float *)0x70003190;
        by = *(float *)0x70003198;
        ay = *(float *)0x70003628;
        lo = *(float *)0x70003600;
        *(float *)0x70003680 = t;
        *(float *)0x70003684 = bx + (ax * t);
        x = *(volatile float *)0x70003684;
        *(float *)0x70003688 = by + (ay * t);
        if (x > lo) {
            if (x < *(float *)0x70003610) {
                y = *(float *)0x70003688;
                if (y > *(float *)0x70003608) {
                    if (y < *(float *)0x70003618) {
                        *(float *)0x700031B0 = x;
                        *(float *)0x700031B4 = box[1];
                        *(float *)0x700031B8 = y;
                        if (face == 3) {
                            *(int *)0x700030D8 = 0x3F800000;
                            *(short *)0x700030CA = 0x4000;
                        } else {
                            *(int *)0x700030D8 = 0xBF800000;
                            *(short *)0x700030CA = (short)0x8000;
                        }
                        *(int *)0x700030DC = 0;
                        *(int *)0x700030D4 = 0;
                        return 1;
                    }
                }
            }
        }
        return 0;
    }
    case 5:
    case 6:
    {
        float dn;
        float t;
        float ax;
        float bx;
        float by;
        float ay;
        float lo;
        float x;
        float y;
        dn = *(float *)0x70003638;
        if (!(dn * (*(float *)0x70003608 - *(float *)0x700031A8) < 0.0f)) {
            return 0;
        }
        t = dn / *(float *)0x70003628;
        ax = *(float *)0x70003620;
        bx = *(float *)0x70003190;
        by = *(float *)0x70003194;
        ay = *(float *)0x70003624;
        lo = *(float *)0x70003600;
        *(float *)0x70003680 = t;
        *(float *)0x70003684 = bx + (ax * t);
        x = *(volatile float *)0x70003684;
        *(float *)0x70003688 = by + (ay * t);
        if (x > lo) {
            if (x < *(float *)0x70003610) {
                y = *(float *)0x70003688;
                if (y > *(float *)0x70003604) {
                    if (y < *(float *)0x70003614) {
                        *(float *)0x700031B0 = x;
                        *(float *)0x700031B4 = y;
                        *(float *)0x700031B8 = box[2];
                        *(short *)0x700030CA = 0x2000;
                        if (face == 5) {
                            *(int *)0x700030DC = 0x3F800000;
                        } else {
                            *(int *)0x700030DC = 0xBF800000;
                        }
                        *(int *)0x700030D8 = 0;
                        *(int *)0x700030D4 = 0;
                        return 1;
                    }
                }
            }
        }
        return 0;
    }
    }
    return 0;
}
