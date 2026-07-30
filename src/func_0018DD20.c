// NEARMISS func_0018DD20  (vram 0x0018DD20, 0x1B48 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 91.21% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation permutation (permuter class), not logic. 1750-instruction function; ~1406/1803 diff rows byte-identical. (a) mwcc hoists the repeated field addresses arg0+0x14/0x50/0x54/0x58 into callee-saved registers (`addiu sN, s6, off` then `0x0(sN)`), while the target always uses direct ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// NEARMISS: objdiff 91.21% vs mwcc 2.3.3 (87.40% vs 991202); ~1406/1803
// instructions byte-identical.  Body/structure recovered in full; the whole
// residual is register allocation, not logic:
//   * mwcc hoists the repeated field addresses (arg0+0x14/0x50/0x54/0x58)
//     into callee-saved registers (addiu sN, s6, off + 0(sN)); the target
//     always uses direct 0xNN(s6) displacement addressing.  That costs one
//     extra callee-saved register, so the frame is 0xE0 here vs 0xC0 in the
//     target and every sN is shifted by one.  Inverse-CSE (inlining the
//     field expression at every use) and `volatile` were both tried and do
//     not defeat the address CSE.
//   * FP register coloring: mwcc pairs fv0/fv1 where the target uses
//     fv0/fv0f, plus a handful of list-scheduler swaps around the compare
//     sequences.
// => permuter territory (regalloc/scheduling permutation), not an idiom.
//
// Player/camera ground+wall collision resolve (called from the movement
// update).  arg0 = actor state block (0x10 = position vec4, 0x20 = previous
// position vec4, 0x50/0x54 = floor/ceiling clamp, 0x58 = surface flags from
// the last probe, 0x5C = crouch scale, 0x90 = facing yaw), arg1 = owner /
// camera block (0xA0 = eye vec4, 0xB0 = look-at vec4), arg2 = movement mode
// (3 = special/ladder), arg3 = collision-world handle.
//
// The 0x700038A0..0x70003A3C scratchpad block is used as the vec4 temp
// bank: D_700038A0/B0/C0/D0/E0/F0, D_70003900/10/20/30/40/50/60 are the
// named vec4 bases (relocated symbols), while individual components are
// written through absolute scratchpad addresses.  D_700031D0 holds the
// pointer to the surface record filled in by func_0019A910 (the ray/sphere
// probe); +0x1A of that record is the surface-flag word (0x8800 = floor-ish,
// 0x2000 = wall, 0x7000 = ceiling group).
//
// Returns a bitmask of what was touched: 1/2/4 = slide planes, 8/0x10 =
// floor/ceiling clamp taken, 0x40/0x80 = clamped to arg0->0x50 / 0x54.

/* vec4 helpers */
extern float func_00102738();                          /* dot */
extern void func_00102760(float *, float *);           /* normalize */
extern void func_001028B8(float *, float *, float *);  /* add */
extern void func_001028D0(float *, float *, float *);  /* sub */
extern void func_00102900(float *, float *, float);    /* scale (variant) */
extern void func_00102948(float *, float *);           /* copy */
extern void func_001031E0(float *, float *);           /* load/expand */
extern void func_00103230(float *, float *, float);    /* scale */
extern float func_0011DE90(float);                     /* cos */
extern float func_0011DF78(float);                     /* fabs-ish */
extern float func_0011E2A8(float);                     /* sin */
extern float func_0011E620(float, float);              /* atan2 */
extern int func_0019A910(float *, float *, int);       /* collision probe */
extern float func_001B1240();                          /* yaw between */
extern float func_001B1470(float);                     /* wrap angle */

extern float D_00810690;
extern unsigned char D_00810700;
extern float D_700031B0;
extern float D_700038A0;
extern float D_700038B0;
extern float D_700038C0;
extern float D_700038D0;
extern float D_700038E0;
extern float D_700038F0;
extern float D_70003900;
extern float D_70003910;
extern float D_70003920;
extern float D_70003930;
extern float D_70003940;
extern float D_70003950;
extern float D_70003960;

int func_0018DD20(char *arg0, char *arg1, int arg2, int arg3) {
    float spA[4];
    float spB[4];
    float temp_f0;
    float temp_f0_10;
    float temp_f0_11;
    float temp_f0_12;
    float temp_f0_13;
    float temp_f0_14;
    float temp_f0_15;
    float temp_f0_2;
    float temp_f0_3;
    float temp_f0_4;
    float temp_f0_5;
    float temp_f0_6;
    float temp_f0_7;
    float temp_f0_8;
    float temp_f0_9;
    float temp_f1;
    float temp_f1_10;
    float temp_f1_11;
    float temp_f1_12;
    float temp_f1_13;
    float temp_f1_14;
    float temp_f1_2;
    float temp_f1_3;
    float temp_f1_4;
    float temp_f1_5;
    float temp_f1_6;
    float temp_f1_7;
    float temp_f1_8;
    float temp_f1_9;
    float temp_f20;
    float var_f0;
    float var_f0_2;
    float var_f0_3;
    float var_f0_4;
    float var_f0_5;
    float var_f0_6;
    float clampY;
    int temp_v0;
    int temp_v0_2;
    int temp_v1_4;
    int temp_v1_6;
    int probeHit;
    int hitA;
    int touched;
    int steep;
    int resolved;
    int hitB;
    int ret;
    int doSlide;
    unsigned short temp_s3;
    unsigned short temp_s7;
    unsigned short temp_v1;
    unsigned short temp_v1_2;
    unsigned short temp_v1_7;
    char *temp_a2;
    char *temp_a2_2;
    char *temp_a2_3;
    char *temp_a2_4;
    char *temp_a2_5;
    char *temp_v1_3;
    char *temp_v1_5;

    touched = 0;
    steep = 0;
    resolved = 0;
    func_00102948(&D_700038B0, (float *)(arg0 + 0x20));
    func_001028D0(&D_700038A0, (float *)(arg0 + 0x10), &D_700038B0);
    func_00102760(&D_700038A0, &D_700038A0);
    func_00103230(&D_700038A0, &D_700038A0, 1.5f);
    func_001028B8(&D_700038A0, &D_700038A0, (float *)(arg0 + 0x10));
    probeHit = func_0019A910(&D_700038B0, &D_700038A0, arg3);
    if (probeHit != 0) {
        touched = 1;
        (*(unsigned short *)((char *)arg0 + 0x58)) = (unsigned short) (*(unsigned short *)((char *)(*(char **)0x700031D0) + 0x1A));
        func_001031E0(&D_700038C0, &D_700031B0);
        func_00102948(&D_70003950, &D_700038C0);
        func_001028D0(&D_700038F0, &D_700038B0, &D_700038A0);
        *(float *)0x700038F4 = 0.0f;
        func_00102760(&D_700038F0, &D_700038F0);
        func_00102948(&D_70003960, &D_700038F0);
        temp_a2 = (*(char **)0x700031D0);
        *(float *)0x700038E0 = (*(float *)((char *)temp_a2 + 0x24));
        *(float *)0x700038E8 = (*(float *)((char *)temp_a2 + 0x2C));
        *(float *)0x700038E4 = 0.0f;
        *(int *)0x700038EC = 0x3F800000;
        temp_f0 = func_00102738(&D_700038F0, &D_700038E0);
        *(float *)0x70003A3C = temp_f0;
        if (temp_f0 < 0.707f) {
            steep = 1;
        }
        *(float *)0x700038E4 = (*(float *)((char *)(*(char **)0x700031D0) + 0x28));
        func_00102760(&D_700038E0, &D_700038E0);
        (*(float *)((char *)arg0 + 0x90)) = func_001B1470(func_0011E620(*(float *)0x700038E0, *(float *)0x700038E8));
        if (arg2 != 3) {
            temp_f0_2 = D_00810690 - func_0011DF78((*(float *)((char *)arg0 + 0xC)));
            *(float *)0x70003A20 = temp_f0_2;
            if (temp_f0_2 <= 0.0f) {
                if ((*(float *)((char *)arg0 + 0x5C)) == 1.0f) {
                    *(float *)0x70003900 = *(float *)0x700038B0;
                    *(float *)0x70003904 = 13.0f + (*(float *)((char *)arg1 + 0xA4));
                    *(float *)0x70003908 = *(float *)0x700038B8;
                } else {
                    *(float *)0x70003900 = *(float *)0x700038B0;
                    *(float *)0x70003904 = (17.5f + (*(float *)((char *)arg1 + 0xA4)));
                    *(float *)0x70003908 = *(float *)0x700038B8;
                }
                *(int *)0x7000390C = 0x3F800000;
                if (func_0019A910(&D_70003900, &D_700038A0, arg3) == 0) {
                    goto block_12;
                }
                if ((short) (*(unsigned short *)((char *)(*(char **)0x700031D0) + 0x1A)) & 0x8800) {
                    func_001028D0(&D_70003900, &D_700031B0, &D_700038A0);
                    temp_f0_3 = func_00102738(&D_70003900, &D_70003900);
                    *(float *)0x70003A20 = temp_f0_3;
                    if (temp_f0_3 < 1.0f) {
block_12:
                        probeHit = 0;
                    }
                }
            }
        }
        if (probeHit != 0) {
            if (arg2 == 3) {
                func_001028D0(&D_700038A0, &D_700038A0, &D_700038B0);
                func_00102760(&D_700038A0, &D_700038A0);
                temp_v1 = (*(unsigned short *)((char *)arg0 + 0x58));
                if (temp_v1 & 0xD800) {
                    if (temp_v1 & 0x8800) {
                        temp_f1 = *(float *)0x700038C4;
                        touched = 8;
                        if (temp_f1 < (*(float *)((char *)arg0 + 0x50))) {
                            (*(float *)((char *)arg0 + 0x50)) = temp_f1;
                        }
                    } else {
                        temp_f1_2 = *(float *)0x700038C4;
                        touched = 0x10;
                        if (!(temp_f1_2 <= (*(float *)((char *)arg0 + 0x54)))) {
                            (*(float *)((char *)arg0 + 0x54)) = temp_f1_2;
                        }
                    }
                    func_00102948((float *)(arg0 + 0x10), &D_700038C0);
                    (*(float *)((char *)arg0 + 0x10)) = ((*(float *)((char *)arg0 + 0x10)) + (0.5f * *(float *)0x700038A0));
                    (*(float *)((char *)arg0 + 0x14)) = ((*(float *)((char *)arg0 + 0x14)) + (0.5f * *(float *)0x700038A4));
                    var_f0 = (*(float *)((char *)arg0 + 0x18)) + (0.5f * *(float *)0x700038A8);
                } else {
                    (*(float *)((char *)arg0 + 0x10)) = *(float *)0x700038C0;
                    (*(float *)((char *)arg0 + 0x18)) = *(float *)0x700038C8;
                    (*(float *)((char *)arg0 + 0x10)) = ((*(float *)((char *)arg0 + 0x10)) + (0.5f * *(float *)0x700038A0));
                    var_f0 = (*(float *)((char *)arg0 + 0x18)) + (0.5f * *(float *)0x700038A8);
                }
                goto block_37;
            }
            temp_v1_2 = (*(unsigned short *)((char *)arg0 + 0x58));
            if (temp_v1_2 & 0x8800) {
                touched = 8;
                func_001028D0(&D_700038A0, &D_700038A0, &D_700038B0);
                func_00102760(&D_700038A0, &D_700038A0);
                func_00102948((float *)(arg0 + 0x10), &D_700038C0);
                (*(float *)((char *)arg0 + 0x14)) = ((*(float *)((char *)arg0 + 0x14)) - 1.0f);
                (*(float *)((char *)arg0 + 0x10)) = ((*(float *)((char *)arg0 + 0x10)) + (0.5f * *(float *)0x700038A0));
                (*(float *)((char *)arg0 + 0x18)) = ((*(float *)((char *)arg0 + 0x18)) + (0.5f * *(float *)0x700038A8));
                temp_f1_3 = (*(float *)((char *)arg0 + 0x14));
                if (temp_f1_3 < (*(float *)((char *)arg0 + 0x50))) {
                    (*(float *)((char *)arg0 + 0x50)) = temp_f1_3;
                }
                temp_f1_4 = (*(float *)((char *)arg0 + 0x14));
                resolved = 1;
                if (!(temp_f1_4 <= (*(float *)((char *)arg0 + 0x54)))) {
                    (*(float *)((char *)arg0 + 0x54)) = temp_f1_4;
                    resolved = 1;
                }
            } else if ((temp_v1_2 & 0x2000) && (func_0019A910(&D_700038A0, &D_700038B0, arg3) != 0)) {
                temp_a2_2 = (*(char **)0x700031D0);
                if ((short) (*(unsigned short *)((char *)temp_a2_2 + 0x1A)) & 0x2000) {
                    *(float *)0x700038D0 = (*(float *)((char *)temp_a2_2 + 0x24));
                    *(float *)0x700038D4 = (*(float *)((char *)temp_a2_2 + 0x28));
                    *(float *)0x700038D8 = (*(float *)((char *)temp_a2_2 + 0x2C));
                    *(int *)0x700038DC = 0x3F800000;
                    temp_f0_4 = func_00102738(&D_700038D0, &D_700038E0);
                    *(float *)0x70003A20 = temp_f0_4;
                    if (!(temp_f0_4 <= -0.3f)) {
                        (*(float *)((char *)arg0 + 0x90)) = func_001B1470(func_0011E620(*(float *)0x700038D0, *(float *)0x700038D8));
                        func_001031E0(&D_700038C0, &D_700031B0);
                        func_00103230(&D_700038D0, &D_700038D0, 4.0f);
                        func_001028B8(&D_700038C0, &D_700038C0, &D_700038D0);
                        resolved = 1;
                        (*(float *)((char *)arg0 + 0x10)) = *(float *)0x700038C0;
                        (*(float *)((char *)arg0 + 0x18)) = *(float *)0x700038C8;
                    }
                }
            }
            if (resolved == 0) {
                func_001028D0(&D_700038A0, &D_700038A0, &D_700038B0);
                func_00102760(&D_700038A0, &D_700038A0);
                (*(float *)((char *)arg0 + 0x10)) = *(float *)0x700038C0;
                (*(float *)((char *)arg0 + 0x18)) = *(float *)0x700038C8;
                (*(float *)((char *)arg0 + 0x10)) = ((*(float *)((char *)arg0 + 0x10)) + (0.5f * *(float *)0x700038A0));
                var_f0 = (*(float *)((char *)arg0 + 0x18)) + (0.5f * *(float *)0x700038A8);
block_37:
                (*(float *)((char *)arg0 + 0x18)) = var_f0;
            }
        }
    }
    doSlide = 1;
    if (steep != 1) {
        doSlide = 0;
        if (probeHit == 0) {
            doSlide = 1;
        }
    }
    if (doSlide != 0) {
        temp_f20 = func_001B1470(func_001B1240((float *)(arg0 + 0x10), (*(float *)((char *)arg0 + 0x20)), (*(float *)((char *)arg0 + 0x28))) - 1.5707964f);
        if (probeHit == 0) {
            *(float *)0x700038A0 = (5.5f * func_0011E2A8(temp_f20));
            *(float *)0x700038A8 = (5.5f * func_0011DE90(temp_f20));
            *(float *)0x700038A4 = 0.0f;
            func_001028B8(&D_700038B0, (float *)(arg0 + 0x10), &D_700038A0);
            func_00102948(&D_70003910, (float *)(arg0 + 0x10));
        } else {
            *(float *)0x700038A0 = (5.5f * func_0011E2A8(temp_f20));
            *(float *)0x700038A8 = (5.5f * func_0011DE90(temp_f20));
            *(float *)0x700038A4 = 0.0f;
            func_001028D0(&D_700038B0, (float *)(arg0 + 0x10), (float *)(arg0 + 0x20));
            func_00102760(&D_700038B0, &D_700038B0);
            func_00103230(&D_700038B0, &D_700038B0, -1.5f);
            func_001028B8(&D_700038B0, &D_700038B0, (float *)(arg0 + 0x10));
            func_001028B8(&D_700038B0, &D_700038B0, &D_700038A0);
            func_00102760(&D_70003910, &D_700038A0);
            func_00103230(&D_70003910, &D_70003910, -3.0f);
            func_001028B8(&D_70003910, (float *)(arg0 + 0x10), &D_70003910);
        }
        func_00102948(spA, (float *)(arg0 + 0x10));
        func_00102948(spB, (float *)(arg0 + 0x10));
        temp_v0 = func_0019A910(&D_70003910, &D_700038B0, arg3);
        hitA = temp_v0;
        if (temp_v0 != 0) {
            func_00102948(&D_70003920, &D_700031B0);
            temp_v1_3 = (*(char **)0x700031D0);
            *(float *)0x70003900 = (*(float *)((char *)temp_v1_3 + 0x24));
            *(float *)0x70003904 = (*(float *)((char *)temp_v1_3 + 0x28));
            *(float *)0x70003908 = (*(float *)((char *)temp_v1_3 + 0x2C));
            *(int *)0x7000390C = 0x3F800000;
            if (probeHit != 0) {
                if (touched & 8) {
                    temp_f0_5 = func_00102738(&D_70003900, &D_70003960);
                    *(float *)0x70003A3C = temp_f0_5;
                    if (temp_f0_5 < -0.08f) {
                        hitA = 0;
                        *(float *)0x70003A3C = -1.0f;
                    }
                } else {
                    temp_f0_6 = func_00102738(&D_70003900, &D_700038E0);
                    *(float *)0x70003A3C = temp_f0_6;
                    if (temp_f0_6 < -0.998f) {
                        func_001028D0(&D_70003910, &D_70003920, &D_700038B0);
                        temp_f0_7 = func_00102738(&D_70003910, &D_70003910);
                        *(float *)0x70003A38 = temp_f0_7;
                        if (temp_f0_7 < 1.0f) {
                            hitA = 0;
                            *(float *)0x70003A3C = -1.0f;
                        } else {
                            *(float *)0x70003910 = *(float *)0x70003950;
                            *(float *)0x70003914 = (*(float *)((char *)arg0 + 0x14));
                            *(float *)0x70003918 = *(float *)0x70003958;
                            *(int *)0x7000391C = 0x3F800000;
                            hitA = func_0019A910(&D_70003910, &D_700038B0, arg3);
                            if (hitA != 0) {
                                func_00102948(&D_70003920, &D_700031B0);
                                temp_a2_3 = (*(char **)0x700031D0);
                                *(float *)0x70003900 = (*(float *)((char *)temp_a2_3 + 0x24));
                                *(float *)0x70003904 = (*(float *)((char *)temp_a2_3 + 0x28));
                                *(float *)0x70003908 = (*(float *)((char *)temp_a2_3 + 0x2C));
                                *(int *)0x7000390C = 0x3F800000;
                                temp_f0_8 = func_00102738(&D_70003900, &D_700038E0);
                                *(float *)0x70003A3C = temp_f0_8;
                                if (temp_f0_8 < -0.998f) {
                                    hitA = 0;
                                    *(float *)0x70003A3C = -1.0f;
                                }
                            } else {
                                hitA = 0;
                                *(float *)0x70003A3C = -1.0f;
                            }
                        }
                    }
                }
            } else {
                *(float *)0x70003A3C = 0.0f;
            }
            if ((hitA != 0) && ((steep == 1) || ((temp_f1_5 = *(float *)0x70003A3C, (temp_f1_5 < 0.9f)) && !(temp_f1_5 <= -0.3f)))) {
                temp_s7 = (*(unsigned short *)((char *)(*(char **)0x700031D0) + 0x1A));
                func_001028D0(&D_700038D0, &D_700038B0, &D_70003910);
                func_001028D0(&D_700038C0, &D_700031B0, &D_700038A0);
                func_00102760(&D_700038D0, &D_700038D0);
                temp_v1_4 = temp_s7 & 0xFFFF;
                if (temp_v1_4 & 0x8800) {
                    *(float *)0x700038F0 = 0.0f;
                    *(float *)0x700038F4 = -1.0f;
                    *(float *)0x700038F8 = 0.0f;
                    *(int *)0x700038FC = 0x3F800000;
                    temp_f0_9 = func_00102738(&D_700038F0, &D_70003900);
                    *(float *)0x70003A3C = temp_f0_9;
                    if (temp_f0_9 < 0.9f) {
                        touched |= 2;
                        spA[0] = *(float *)0x700038C0;
                        spA[2] = *(float *)0x700038C8;
                        spA[0] += 0.1f * *(float *)0x700038D0;
                        spA[2] += 0.1f * *(float *)0x700038D8;
                    } else {
                        func_00102948(spA, &D_700038C0);
                        touched |= 0xA;
                        spA[0] += 0.1f * *(float *)0x700038D0;
                        var_f0_2 = spA[2] + (0.1f * *(float *)0x700038D8);
                        goto block_74;
                    }
                } else if (temp_v1_4 & 0x2000) {
                    touched |= 2;
                    spA[0] = *(float *)0x700038C0;
                    spA[2] = *(float *)0x700038C8;
                    spA[0] += 0.1f * *(float *)0x700038D0;
                    spA[2] += 0.1f * *(float *)0x700038D8;
                } else {
                    func_00102948(spA, &D_700038C0);
                    touched |= 3;
                    spA[0] += 0.1f * *(float *)0x700038D0;
                    var_f0_2 = spA[2] + (0.1f * *(float *)0x700038D8);
block_74:
                    spA[2] = var_f0_2;
                }
            }
        }
        if (probeHit == 0) {
            func_001028D0(&D_700038B0, (float *)(arg0 + 0x10), &D_700038A0);
            func_00102948(&D_70003910, (float *)(arg0 + 0x10));
        } else {
            func_001028D0(&D_700038B0, (float *)(arg0 + 0x10), (float *)(arg0 + 0x20));
            func_00102760(&D_700038B0, &D_700038B0);
            func_00103230(&D_700038B0, &D_700038B0, -1.5f);
            func_001028B8(&D_700038B0, &D_700038B0, (float *)(arg0 + 0x10));
            func_001028D0(&D_700038B0, &D_700038B0, &D_700038A0);
            func_00102760(&D_70003910, &D_700038A0);
            func_00103230(&D_70003910, &D_70003910, 3.0f);
            func_001028B8(&D_70003910, (float *)(arg0 + 0x10), &D_70003910);
        }
        temp_v0_2 = func_0019A910(&D_70003910, &D_700038B0, arg3);
        hitB = temp_v0_2;
        if (temp_v0_2 != 0) {
            func_00102948(&D_70003930, &D_700031B0);
            temp_v1_5 = (*(char **)0x700031D0);
            *(float *)0x700038F0 = (*(float *)((char *)temp_v1_5 + 0x24));
            *(float *)0x700038F4 = (*(float *)((char *)temp_v1_5 + 0x28));
            *(float *)0x700038F8 = (*(float *)((char *)temp_v1_5 + 0x2C));
            *(int *)0x700038FC = 0x3F800000;
            if (probeHit != 0) {
                if (touched & 8) {
                    temp_f0_10 = func_00102738(&D_700038F0, &D_70003960);
                    *(float *)0x70003A3C = temp_f0_10;
                    if (temp_f0_10 < -0.08f) {
                        steep = 0;
                        hitB = 0;
                        *(float *)0x70003A3C = -1.0f;
                    }
                } else {
                    temp_f0_11 = func_00102738(&D_700038F0, &D_700038E0);
                    *(float *)0x70003A3C = temp_f0_11;
                    if (temp_f0_11 < -0.998f) {
                        func_001028D0(&D_70003910, &D_70003930, &D_700038B0);
                        temp_f0_12 = func_00102738(&D_70003910, &D_70003910);
                        *(float *)0x70003A38 = temp_f0_12;
                        if (temp_f0_12 < 1.0f) {
                            steep = 0;
                            hitB = 0;
                            *(float *)0x70003A3C = -1.0f;
                        } else {
                            *(float *)0x70003910 = *(float *)0x70003950;
                            *(float *)0x70003914 = (*(float *)((char *)arg0 + 0x14));
                            *(float *)0x70003918 = *(float *)0x70003958;
                            *(int *)0x7000391C = 0x3F800000;
                            hitB = func_0019A910(&D_70003910, &D_700038B0, arg3);
                            if (hitB != 0) {
                                func_00102948(&D_70003930, &D_700031B0);
                                temp_a2_4 = (*(char **)0x700031D0);
                                *(float *)0x700038F0 = (*(float *)((char *)temp_a2_4 + 0x24));
                                *(float *)0x700038F4 = (*(float *)((char *)temp_a2_4 + 0x28));
                                *(float *)0x700038F8 = (*(float *)((char *)temp_a2_4 + 0x2C));
                                *(int *)0x700038FC = 0x3F800000;
                                temp_f0_13 = func_00102738(&D_700038F0, &D_700038E0);
                                *(float *)0x70003A3C = temp_f0_13;
                                if (temp_f0_13 < -0.998f) {
                                    steep = 0;
                                    hitB = 0;
                                    *(float *)0x70003A3C = -1.0f;
                                }
                            } else {
                                steep = 0;
                                hitB = 0;
                                *(float *)0x70003A3C = -1.0f;
                            }
                        }
                    }
                }
            } else {
                *(float *)0x70003A3C = 0.0f;
            }
            if (steep != 1) {
                temp_f1_6 = *(float *)0x70003A3C;
                if (temp_f1_6 < 0.9f) {
                    if (!(temp_f1_6 <= -0.3f)) {
                        goto block_95;
                    }
                }
            } else {
block_95:
                temp_s3 = (*(unsigned short *)((char *)(*(char **)0x700031D0) + 0x1A));
                func_001028D0(&D_700038D0, &D_700038B0, &D_70003910);
                func_001028B8(&D_700038C0, &D_700031B0, &D_700038A0);
                func_00102760(&D_700038D0, &D_700038D0);
                temp_v1_6 = temp_s3 & 0xFFFF;
                if (temp_v1_6 & 0x8800) {
                    *(float *)0x700038B0 = 0.0f;
                    *(float *)0x700038B4 = -1.0f;
                    *(float *)0x700038B8 = 0.0f;
                    *(int *)0x700038BC = 0x3F800000;
                    temp_f0_14 = func_00102738(&D_700038B0, &D_700038F0);
                    *(float *)0x70003A3C = temp_f0_14;
                    if (temp_f0_14 < 0.9f) {
                        touched |= 4;
                        spB[0] = *(float *)0x700038C0;
                        spB[2] = *(float *)0x700038C8;
                        spB[0] += 0.1f * *(float *)0x700038D0;
                        spB[2] += 0.1f * *(float *)0x700038D8;
                    } else {
                        func_00102948(spB, &D_700038C0);
                        touched |= 8;
                        spB[0] += 0.1f * *(float *)0x700038D0;
                        var_f0_3 = spB[2] + (0.1f * *(float *)0x700038D8);
                        goto block_103;
                    }
                } else if (temp_v1_6 & 0x2000) {
                    touched |= 4;
                    spB[0] = *(float *)0x700038C0;
                    spB[2] = *(float *)0x700038C8;
                    spB[0] += 0.1f * *(float *)0x700038D0;
                    spB[2] += 0.1f * *(float *)0x700038D8;
                } else {
                    func_00102948(spB, &D_700038C0);
                    touched |= 1;
                    spB[0] += 0.1f * *(float *)0x700038D0;
                    var_f0_3 = spB[2] + (0.1f * *(float *)0x700038D8);
block_103:
                    spB[2] = var_f0_3;
                }
            }
        }
        if ((touched & 6) == 6) {
            func_001028B8(&D_70003940, &D_70003920, &D_70003930);
            func_00103230(&D_70003940, &D_70003940, 0.5f);
            (*(float *)((char *)arg0 + 0x10)) = *(float *)0x70003940;
            (*(float *)((char *)arg0 + 0x18)) = *(float *)0x70003948;
        } else if (touched & 2) {
            if (hitB != 0) {
                func_001028B8(&D_70003940, &D_70003920, &D_70003930);
                func_00103230(&D_70003940, &D_70003940, 0.5f);
                (*(float *)((char *)arg0 + 0x10)) = *(float *)0x70003940;
                (*(float *)((char *)arg0 + 0x18)) = *(float *)0x70003948;
            } else {
                func_00102948((float *)(arg0 + 0x10), spA);
            }
        } else if (touched & 4) {
            if (hitA != 0) {
                func_001028B8(&D_70003940, &D_70003920, &D_70003930);
                func_00103230(&D_70003940, &D_70003940, 0.5f);
                (*(float *)((char *)arg0 + 0x10)) = *(float *)0x70003940;
                (*(float *)((char *)arg0 + 0x18)) = *(float *)0x70003948;
            } else {
                func_00102948((float *)(arg0 + 0x10), spB);
            }
        }
    }
    if (D_00810700 == 0x12) {
        temp_f1_7 = (*(float *)((char *)arg0 + 0x18));
        clampY = 169.5f;
        if (temp_f1_7 < 169.5f) {
            goto block_123;
        }
        clampY = 230.6f;
        if (!(temp_f1_7 <= 230.6f)) {
block_123:
            (*(float *)((char *)arg0 + 0x18)) = clampY;
        }
    }
    if (probeHit != 0) {
        temp_v1_7 = (*(unsigned short *)((char *)arg0 + 0x58));
        if (!(temp_v1_7 & 0x2000)) {
            if (temp_v1_7 & 0x8800) {
                var_f0_4 = *(float *)0x70003954 - 1.0f;
                if (!((*(float *)((char *)arg0 + 0x14)) <= var_f0_4)) {
                    goto block_130;
                }
            } else {
                var_f0_4 = 1.0f + *(float *)0x70003954;
                if ((*(float *)((char *)arg0 + 0x14)) < var_f0_4) {
block_130:
                    (*(float *)((char *)arg0 + 0x14)) = var_f0_4;
                }
            }
        }
    }
    temp_f1_8 = (*(float *)((char *)arg0 + 0x50));
    if ((*(float *)((char *)arg0 + 0x14)) <= temp_f1_8) {
        (*(float *)((char *)arg0 + 0x14)) = temp_f1_8;
    }
    temp_f1_9 = (*(float *)((char *)arg0 + 0x54));
    if (!((*(float *)((char *)arg0 + 0x14)) < temp_f1_9)) {
        (*(float *)((char *)arg0 + 0x14)) = temp_f1_9;
    }
    func_00102948(&D_700038F0, (float *)(arg1 + 0xA0));
    *(float *)0x700038F4 = (*(float *)0x700038F4 + 11.0f);
    func_001028D0(&D_700038F0, (float *)(arg0 + 0x10), &D_700038F0);
    func_00102760(&D_700038F0, &D_700038F0);
    func_00102900(&D_700038F0, &D_700038F0, 1.5f);
    func_001028D0(&D_700038F0, (float *)(arg0 + 0x10), &D_700038F0);
    *(float *)0x700038A0 = *(float *)0x700038F0;
    *(float *)0x700038A4 = (*(float *)0x700038F4 - 200.0f);
    *(float *)0x700038A8 = *(float *)0x700038F8;
    *(int *)0x700038AC = 0x3F800000;
    if ((func_0019A910(&D_700038F0, &D_700038A0, arg3) != 0) && ((short) (*(unsigned short *)((char *)(*(char **)0x700031D0) + 0x1A)) & 0x7000)) {
        *(float *)0x70003A38 = *(float *)0x700031B4;
        if ((*(float *)((char *)arg0 + 0x5C)) == 1.0f) {
            var_f0_5 = *(float *)0x70003A38 + 6.0f;
        } else {
            var_f0_5 = *(float *)0x70003A38 + 17.0f;
        }
    } else {
        var_f0_5 = (*(float *)((char *)arg0 + 0x50)) - 200.0f;
    }
    *(float *)0x70003A38 = var_f0_5;
    *(float *)0x700038B0 = *(float *)0x700038F0;
    *(float *)0x700038B4 = (200.0f + *(float *)0x700038F4);
    *(float *)0x700038B8 = *(float *)0x700038F8;
    *(int *)0x700038BC = 0x3F800000;
    if ((func_0019A910(&D_700038F0, &D_700038B0, arg3) != 0) && ((short) (*(unsigned short *)((char *)(*(char **)0x700031D0) + 0x1A)) & 0x8800)) {
        temp_f1_10 = *(float *)0x700031B4;
        var_f0_6 = temp_f1_10 - 1.0f;
        *(float *)0x70003A3C = temp_f1_10;
        goto block_150;
    }
    if (D_00810700 == 0x12) {
        *(float *)0x700038B0 = (*(float *)((char *)arg1 + 0xB0));
        *(float *)0x700038B4 = (200.0f + (*(float *)((char *)arg1 + 0xB4)));
        *(float *)0x700038B8 = (*(float *)((char *)arg1 + 0xB8));
        *(int *)0x700038BC = 0x3F800000;
        if (func_0019A910((float *)(arg1 + 0xB0), &D_700038B0, arg3) != 0) {
            *(float *)0x700038B0 = 0.0f;
            *(float *)0x700038B4 = -1.0f;
            *(float *)0x700038B8 = 0.0f;
            *(int *)0x700038BC = 0x3F800000;
            temp_a2_5 = (*(char **)0x700031D0);
            *(float *)0x700038F0 = (*(float *)((char *)temp_a2_5 + 0x24));
            *(float *)0x700038F4 = (*(float *)((char *)temp_a2_5 + 0x28));
            *(float *)0x700038F8 = (*(float *)((char *)temp_a2_5 + 0x2C));
            *(int *)0x700038FC = 0x3F800000;
            temp_f0_15 = func_00102738(&D_700038B0, &D_700038F0);
            *(float *)0x70003A3C = temp_f0_15;
            if (!(temp_f0_15 <= 0.2f)) {
                temp_f1_11 = *(float *)0x700031B4;
                var_f0_6 = temp_f1_11 - 1.0f;
                *(float *)0x70003A3C = temp_f1_11;
            } else {
                var_f0_6 = 200.0f + (*(float *)((char *)arg0 + 0x14));
            }
            goto block_150;
        }
    } else {
        var_f0_6 = 200.0f + (*(float *)((char *)arg0 + 0x14));
block_150:
        *(float *)0x70003A3C = var_f0_6;
    }
    temp_f1_12 = *(float *)0x70003A3C;
    if (!(*(float *)0x70003A38 <= temp_f1_12)) {
        *(float *)0x70003A38 = (temp_f1_12 - 3.0f);
    }
    (*(float *)((char *)arg0 + 0x50)) = *(float *)0x70003A38;
    (*(float *)((char *)arg0 + 0x54)) = *(float *)0x70003A3C;
    if ((D_00810700 == 0x15) && !((*(float *)((char *)arg0 + 0x18)) <= 260.0f)) {
        (*(float *)((char *)arg0 + 0x50)) = 70.0f;
    }
    temp_f1_13 = (*(float *)((char *)arg0 + 0x50));
    if ((*(float *)((char *)arg0 + 0x14)) <= temp_f1_13) {
        (*(float *)((char *)arg0 + 0x14)) = temp_f1_13;
        touched |= 0x40;
    }
    temp_f1_14 = (*(float *)((char *)arg0 + 0x54));
    ret = touched;
    if (!((*(float *)((char *)arg0 + 0x14)) < temp_f1_14)) {
        (*(float *)((char *)arg0 + 0x14)) = temp_f1_14;
        ret = touched | 0x80;
    }
    return ret;
}
