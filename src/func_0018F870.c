// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x700038B0 0x700038B8 0x70003950 0x70003958 0x700038C8 0x700038D0 0x700038D8 0x70003900 0x70003904 0x70003910 0x70003914 0x70003A38 0x700038C4 0x700038F4 0x70003954 0x700038A0 0x700038C0 0x700038F8 0x700038A4 0x700038B4
// SPAD: 0x700031D0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 94.15% NEARMISS).
// Fixes read from the original instructions (spellings follow the byte-matched
// twin func_0018DD20):
//  - The scratchpad scalars/vectors are relocated externs (idiom-32, the // SPAD:
//    lines, same set as the twin), including the collision-result pointer
//    D_700031D0; the remaining scratchpad words stay plain (non-volatile) literal
//    accesses, which pins their store order. The old volatile literals let the
//    0x7000 upper half drift into branch delay slots.
//  - The mode select is `anim == 6 ? 0 : (blocked == 1 || hit0 == 0) ? 1 : 2`
//    written as an if / else-if chain.
//  - Both side probes keep the raw return in its own temporary (rA / rB) and test
//    it, copying it to hitA / hitB (the original branches on the return register).
//  - The side-probe "use this hit" tests are `if (blocked != 1) { if (h < 0.9 &&
//    !(h <= -0.3)) goto resolve; } else { resolve: ... }`, as in the twin.
//  - Compound `+=` for the 0x700038F4 bump and the 0x70003A38 floor offset; the
//    0x700031B4 hit height is read once into a temporary before the two stores
//    to 0x70003A3C; the final ceiling compare re-reads 0x70003A3C.
//
// Camera collision / wall-slide resolve.  `self` is the camera block,
// `other` the tracked actor (its focus point is at other+0xB0).
//
// Step 1: cast from the actor focus toward the camera eye (self+0x10),
// extended by 1.5 units (func_00103230 scale), through func_0019A910.
// A hit records the surface flags (hit+0x1A) in self+0x58 and, depending on
// whether the surface is a floor/ceiling (0x8800) or a wall, clamps the
// camera height limits (self+0x50 / self+0x54) or snaps the eye to the hit
// point plus half the push-out vector.
//
// Step 2: depending on `mode` (0 = no probe for anim 6, 1 = normal,
// 2 = "blocked" re-probe), two side probes are cast 5.5 units left and
// right of the view direction; each hit that is not nearly parallel to the
// view accumulates a bit in `flags` and a candidate eye position in
// bufA / bufB.  With both sides blocked the midpoint of the two hit points
// is used; with only one side blocked the corresponding candidate is used.
//
// Step 3: two vertical probes (+-200) establish the ceiling/floor slack
// stored in self+0x50 / self+0x54.
//
// Returns the accumulated `flags` bitmask.
extern void func_00102948(void *dst, void *src);          /* vec copy   */
extern void func_001028D0(void *dst, void *a, void *b);   /* dst = a - b */
extern void func_001028B8(void *dst, void *a, void *b);   /* dst = a + b */
extern void func_00102760(void *dst, void *src);          /* normalize  */
extern void func_00103230(void *dst, void *src, float k); /* scale      */
extern void func_001031E0(void *dst, void *src);
extern float func_00102738(void *a, void *b);             /* dot        */
extern int func_0019A910(void *from, void *to, int world);
extern void func_0018CE60(unsigned char *self, void *v, int anim);
extern float func_0011DE90(float);                        /* cos */
extern float func_0011E2A8(float);                        /* sin */
extern float func_0011E620(float, float);                 /* atan2 */
extern float func_001B1470(float);                        /* angle wrap */
extern float func_001B1240(void *, float, float);
extern unsigned char D_00810700;
extern unsigned char *D_700031D0;
extern float D_700031B0;
extern float D_700038A0;
extern float D_700038A4;
extern float D_700038B0;
extern float D_700038B4;
extern float D_700038B8;
extern float D_700038C0;
extern float D_700038C4;
extern float D_700038C8;
extern float D_700038D0;
extern float D_700038D8;
extern float D_700038E0;
extern float D_700038F0;
extern float D_700038F4;
extern float D_700038F8;
extern float D_70003900;
extern float D_70003904;
extern float D_70003910;
extern float D_70003920;
extern float D_70003930;
extern float D_70003940;
extern float D_70003950;
extern float D_70003954;
extern float D_70003960;
extern float D_700039F0;
extern float D_70003A38;

int func_0018F870(unsigned char *self, unsigned char *other, int anim, int world)
{
    float bufA[4];
    float bufB[4];
    float ang;
    float dot;
    float h;
    int hit0;
    int hitA;
    int flags;
    int blocked;
    int hitB;
    int mode;
    unsigned short sflags;
    unsigned char *hit;
    int rA;
    int rB;

    flags = 0;
    blocked = 0;
    func_00102948(&D_700038B0, other + 0xB0);
    func_001028D0(&D_700038A0, self + 0x10, &D_700038B0);
    func_00102760(&D_700038A0, &D_700038A0);
    func_00103230(&D_700038A0, &D_700038A0, 1.5f);
    func_001028B8(&D_700038A0, &D_700038A0, self + 0x10);
    hit0 = func_0019A910(&D_700038B0, &D_700038A0, world);
    if (hit0 != 0) {
        flags = 1;
        hit = D_700031D0;
        *(unsigned short *)(self + 0x58) = *(unsigned short *)(hit + 0x1A);
        func_001031E0(&D_700038C0, &D_700031B0);
        func_00102948(&D_70003950, &D_700038C0);
        func_001028D0(&D_700038F0, self + 0x20, self + 0x10);
        D_700038F4 = 0.0f;
        func_00102760(&D_700038F0, &D_700038F0);
        func_001028D0(&D_70003960, other + 0xB0, self + 0x10);
        *(float *)0x70003964 = 0.0f;
        func_00102760(&D_70003960, &D_70003960);
        hit = D_700031D0;
        *(float *)0x700038E0 = *(float *)(hit + 0x24);
        *(float *)0x700038E8 = *(float *)(hit + 0x2C);
        *(float *)0x700038E4 = 0.0f;
        *(float *)0x700038EC = 1.0f;
        dot = func_00102738(&D_700038F0, &D_700038E0);
        *(float *)0x70003A3C = dot;
        if (dot < 0.99f) {
            blocked = 1;
        }
        *(float *)0x700038E4 = *(float *)(D_700031D0 + 0x28);
        func_001028D0(&D_700038A0, &D_700038A0, &D_700038B0);
        func_00102760(&D_700038A0, &D_700038A0);
        sflags = *(unsigned short *)(self + 0x58);
        if (sflags & 0xD800) {
            if (sflags & 0x8800) {
                h = D_700038C4;
                flags = 8;
                if (h < *(float *)(self + 0x50)) {
                    *(float *)(self + 0x50) = h;
                }
            } else {
                h = D_700038C4;
                flags = 0x10;
                if (!(h <= *(float *)(self + 0x54))) {
                    *(float *)(self + 0x54) = h;
                }
            }
            func_00102948(self + 0x10, &D_700038C0);
            *(float *)(self + 0x10) = *(float *)(self + 0x10)
                + 0.5f * D_700038A0;
            *(float *)(self + 0x18) = *(float *)(self + 0x18)
                + 0.5f * *(float *)0x700038A8;
        } else {
            *(float *)(self + 0x10) = D_700038C0;
            *(float *)(self + 0x18) = D_700038C8;
            *(float *)(self + 0x10) = *(float *)(self + 0x10)
                + 0.5f * D_700038A0;
            *(float *)(self + 0x18) = *(float *)(self + 0x18)
                + 0.5f * *(float *)0x700038A8;
            if (hit0 == 4) {
                func_00102948(&D_700038D0, &D_700038C0);
                func_001028D0(&D_700038D0, &D_700038D0, &D_700038A0);
                func_0018CE60(self, &D_700038D0, anim);
            } else {
                if (*(float *)(self + 0x14) <= *(float *)(self + 0x50)) {
                    *(float *)(self + 0x14) = *(float *)(self + 0x50);
                }
                if (!(*(float *)(self + 0x14) < *(float *)(self + 0x54))) {
                    *(float *)(self + 0x14) = *(float *)(self + 0x54);
                }
            }
        }
    }
    if (anim == 6) {
        mode = 0;
    } else if (blocked == 1 || hit0 == 0) {
        mode = 1;
    } else {
        mode = 2;
    }
    if (mode == 2) {
        ang = func_001B1470(func_0011E620(*(float *)0x700038E0,
                                          *(float *)0x700038E8) - 1.5707964f);
        func_001028B8(&D_700039F0, self + 0x10, &D_700038E0);
        D_700038A0 = 5.5f * func_0011E2A8(ang);
        *(float *)0x700038A8 = 5.5f * func_0011DE90(ang);
        D_700038A4 = 0.0f;
        func_001028B8(&D_700038B0, &D_700039F0, &D_700038A0);
        func_00102760(&D_70003910, &D_700038A0);
        func_00103230(&D_70003910, &D_70003910, -3.0f);
        func_001028B8(&D_70003910, &D_700039F0, &D_70003910);
        if (func_0019A910(&D_70003910, &D_700038B0, world) != 0) {
            hit = D_700031D0;
            if (*(short *)(hit + 0x1A) & 0x2000) {
                D_70003900 = *(float *)(hit + 0x24);
                D_70003904 = *(float *)(hit + 0x28);
                *(float *)0x70003908 = *(float *)(hit + 0x2C);
                *(float *)0x7000390C = 1.0f;
                dot = func_00102738(&D_70003900, &D_700038E0);
                *(float *)0x70003A3C = dot;
                if (dot < 0.9f) {
                    func_001028D0(&D_700038C0, &D_700031B0, &D_700038A0);
                    flags |= 2;
                    *(float *)(self + 0x10) = D_700038C0;
                    *(float *)(self + 0x18) = D_700038C8;
                }
            }
        }
        if ((flags & 2) == 0) {
            func_001028D0(&D_700038B0, &D_700039F0, &D_700038A0);
            func_00102760(&D_70003910, &D_700038A0);
            func_00103230(&D_70003910, &D_70003910, 3.0f);
            func_001028B8(&D_70003910, &D_700039F0, &D_70003910);
            if (func_0019A910(&D_70003910, &D_700038B0, world) != 0) {
                hit = D_700031D0;
                if (*(short *)(hit + 0x1A) & 0x2000) {
                    D_70003900 = *(float *)(hit + 0x24);
                    D_70003904 = *(float *)(hit + 0x28);
                    *(float *)0x70003908 = *(float *)(hit + 0x2C);
                    *(float *)0x7000390C = 1.0f;
                    dot = func_00102738(&D_70003900, &D_700038E0);
                    *(float *)0x70003A3C = dot;
                    if (dot < 0.9f) {
                        func_001028B8(&D_700038C0, &D_700031B0, &D_700038A0);
                        flags |= 4;
                        *(float *)(self + 0x10) = D_700038C0;
                        *(float *)(self + 0x18) = D_700038C8;
                    }
                }
            }
        }
    } else if (mode == 1) {
        ang = func_001B1470(func_001B1240(self + 0x10, *(float *)(self + 0x20),
                                          *(float *)(self + 0x28)) - 1.5707964f);
        if (hit0 == 0) {
            D_700038A0 = 5.5f * func_0011E2A8(ang);
            *(float *)0x700038A8 = 5.5f * func_0011DE90(ang);
            D_700038A4 = 0.0f;
            func_001028B8(&D_700038B0, self + 0x10, &D_700038A0);
            func_00102948(&D_70003910, self + 0x10);
        } else {
            D_700038A0 = 5.5f * func_0011E2A8(ang);
            *(float *)0x700038A8 = 5.5f * func_0011DE90(ang);
            D_700038A4 = 0.0f;
            func_001028B8(&D_700038B0, self + 0x10, &D_700038A0);
            func_00102760(&D_70003910, &D_700038A0);
            func_00103230(&D_70003910, &D_70003910, -3.0f);
            func_001028B8(&D_70003910, self + 0x10, &D_70003910);
        }
        func_00102948(bufA, self + 0x10);
        func_00102948(bufB, self + 0x10);
        rA = func_0019A910(&D_70003910, &D_700038B0, world);
        hitA = rA;
        if (rA != 0) {
            func_00102948(&D_70003920, &D_700031B0);
            hit = D_700031D0;
            D_70003900 = *(float *)(hit + 0x24);
            D_70003904 = *(float *)(hit + 0x28);
            *(float *)0x70003908 = *(float *)(hit + 0x2C);
            *(float *)0x7000390C = 1.0f;
            if (hit0 != 0) {
                if (flags & 8) {
                    dot = func_00102738(&D_70003900, &D_70003960);
                    *(float *)0x70003A3C = dot;
                    if (dot < -0.08f) {
                        hitA = 0;
                        *(float *)0x70003A3C = -1.0f;
                    }
                } else {
                    dot = func_00102738(&D_70003900, &D_700038E0);
                    *(float *)0x70003A3C = dot;
                    if (dot < -0.998f) {
                        hitA = 0;
                        *(float *)0x70003A3C = -1.0f;
                    }
                }
            } else {
                *(float *)0x70003A3C = 0.0f;
            }
            if (hitA != 0) {
                if (blocked != 1) {
                    h = *(float *)0x70003A3C;
                    if (h < 0.9f) {
                        if (!(h <= -0.3f)) {
                            goto resolveA;
                        }
                    }
                } else {
resolveA:
                    sflags = *(unsigned short *)(D_700031D0 + 0x1A);
                    func_001028D0(&D_700038D0, &D_700038B0, &D_70003910);
                    func_001028D0(&D_700038C0, &D_700031B0, &D_700038A0);
                    func_00102760(&D_700038D0, &D_700038D0);
                    if (sflags & 0x8800) {
                        *(float *)0x700038F0 = 0.0f;
                        D_700038F4 = -1.0f;
                        *(float *)(0x700038F8) = 0.0f;
                        *(float *)0x700038FC = 1.0f;
                        dot = func_00102738(&D_700038F0, &D_70003900);
                        *(float *)0x70003A3C = dot;
                        if (dot < 0.9f) {
                            flags |= 2;
                            bufA[0] = D_700038C0;
                            bufA[2] = D_700038C8;
                            bufA[0] += 0.1f * D_700038D0;
                            bufA[2] += 0.1f * D_700038D8;
                        } else {
                            func_00102948(bufA, &D_700038C0);
                            flags |= 0xA;
                            bufA[0] += 0.1f * D_700038D0;
                            bufA[2] += 0.1f * D_700038D8;
                        }
                    } else if (sflags & 0x2000) {
                        flags |= 2;
                        bufA[0] = D_700038C0;
                        bufA[2] = D_700038C8;
                        bufA[0] += 0.1f * D_700038D0;
                        bufA[2] += 0.1f * D_700038D8;
                    } else {
                        func_00102948(bufA, &D_700038C0);
                        flags |= 3;
                        bufA[0] += 0.1f * D_700038D0;
                        bufA[2] += 0.1f * D_700038D8;
                    }
                }
            }
        }
        if (hit0 == 0) {
            func_001028D0(&D_700038B0, self + 0x10, &D_700038A0);
            func_00102948(&D_70003910, self + 0x10);
        } else {
            func_001028D0(&D_700038B0, self + 0x10, &D_700038A0);
            func_00102760(&D_70003910, &D_700038A0);
            func_00103230(&D_70003910, &D_70003910, 3.0f);
            func_001028B8(&D_70003910, self + 0x10, &D_70003910);
        }
        rB = func_0019A910(&D_70003910, &D_700038B0, world);
        hitB = rB;
        if (rB != 0) {
            func_00102948(&D_70003930, &D_700031B0);
            hit = D_700031D0;
            *(float *)0x700038F0 = *(float *)(hit + 0x24);
            D_700038F4 = *(float *)(hit + 0x28);
            D_700038F8 = *(float *)(hit + 0x2C);
            *(float *)0x700038FC = 1.0f;
            if (hit0 != 0) {
                if (flags & 8) {
                    dot = func_00102738(&D_700038F0, &D_70003960);
                    *(float *)0x70003A3C = dot;
                    if (dot < -0.08f) {
                        blocked = 0;
                        hitB = 0;
                        *(float *)0x70003A3C = -1.0f;
                    }
                } else {
                    dot = func_00102738(&D_700038F0, &D_700038E0);
                    *(float *)0x70003A3C = dot;
                    if (dot < -0.998f) {
                        blocked = 0;
                        hitB = 0;
                        *(float *)0x70003A3C = -1.0f;
                    }
                }
            } else {
                *(float *)0x70003A3C = 0.0f;
            }
            if (blocked != 1) {
                h = *(float *)0x70003A3C;
                if (h < 0.9f) {
                    if (!(h <= -0.3f)) {
                        goto resolveB;
                    }
                }
            } else {
resolveB:
                sflags = *(unsigned short *)(D_700031D0 + 0x1A);
                func_001028D0(&D_700038D0, &D_700038B0, &D_70003910);
                func_001028B8(&D_700038C0, &D_700031B0, &D_700038A0);
                func_00102760(&D_700038D0, &D_700038D0);
                if (sflags & 0x8800) {
                    D_700038B0 = 0.0f;
                    D_700038B4 = -1.0f;
                    *(float *)(0x700038B8) = 0.0f;
                    *(float *)0x700038BC = 1.0f;
                    dot = func_00102738(&D_700038B0, &D_700038F0);
                    *(float *)0x70003A3C = dot;
                    if (dot < 0.9f) {
                        flags |= 4;
                        bufB[0] = D_700038C0;
                        bufB[2] = D_700038C8;
                        bufB[0] += 0.1f * D_700038D0;
                        bufB[2] += 0.1f * D_700038D8;
                    } else {
                        func_00102948(bufB, &D_700038C0);
                        flags |= 8;
                        bufB[0] += 0.1f * D_700038D0;
                        bufB[2] += 0.1f * D_700038D8;
                    }
                } else if (sflags & 0x2000) {
                    flags |= 4;
                    bufB[0] = D_700038C0;
                    bufB[2] = D_700038C8;
                    bufB[0] += 0.1f * D_700038D0;
                    bufB[2] += 0.1f * D_700038D8;
                } else {
                    func_00102948(bufB, &D_700038C0);
                    flags |= 1;
                    bufB[0] += 0.1f * D_700038D0;
                    bufB[2] += 0.1f * D_700038D8;
                }
            }
        }
        if ((flags & 6) == 6) {
            func_001028B8(&D_70003940, &D_70003920, &D_70003930);
            func_00103230(&D_70003940, &D_70003940, 0.5f);
            *(float *)(self + 0x10) = *(float *)0x70003940;
            *(float *)(self + 0x18) = *(float *)0x70003948;
        } else if (flags & 2) {
            if (hitB != 0) {
                func_001028B8(&D_70003940, &D_70003920, &D_70003930);
                func_00103230(&D_70003940, &D_70003940, 0.5f);
                *(float *)(self + 0x10) = *(float *)0x70003940;
                *(float *)(self + 0x18) = *(float *)0x70003948;
            } else {
                func_00102948(self + 0x10, bufA);
            }
        } else if (flags & 4) {
            if (hitA != 0) {
                func_001028B8(&D_70003940, &D_70003920, &D_70003930);
                func_00103230(&D_70003940, &D_70003940, 0.5f);
                *(float *)(self + 0x10) = *(float *)0x70003940;
                *(float *)(self + 0x18) = *(float *)0x70003948;
            } else {
                func_00102948(self + 0x10, bufB);
            }
        }
    }
    if (D_00810700 == 0x12) {
        if (*(float *)(self + 0x18) < 169.5f) {
            *(float *)(self + 0x18) = 169.5f;
        } else if (!(*(float *)(self + 0x18) <= 230.6f)) {
            *(float *)(self + 0x18) = 230.6f;
        }
    }
    if (hit0 != 0) {
        sflags = *(unsigned short *)(self + 0x58);
        if (!(sflags & 0x2000)) {
            if (sflags & 0x8800) {
                if (!(*(float *)(self + 0x14) <= D_70003954 - 1.0f)) {
                    *(float *)(self + 0x14) = D_70003954 - 1.0f;
                }
            } else {
                if (*(float *)(self + 0x14) < 1.0f + D_70003954) {
                    *(float *)(self + 0x14) = 1.0f + D_70003954;
                }
            }
        }
    }
    if ((flags & 0x1F) != 0) {
        func_00102948(&D_700038B0, other + 0xB0);
        func_00102948(&D_700038A0, self + 0x10);
        if (func_0019A910(&D_700038B0, self + 0x10, world) != 0) {
            func_00102948(&D_700038C0, &D_700031B0);
            *(float *)(self + 0x10) = D_700038C0;
            *(float *)(self + 0x18) = D_700038C8;
        }
    }
    func_00102948(&D_700038F0, other + 0xA0);
    D_700038F4 += 11.0f;
    func_001028D0(&D_700038F0, self + 0x10, &D_700038F0);
    func_00102760(&D_700038F0, &D_700038F0);
    func_001028D0(&D_700038F0, self + 0x10, &D_700038F0);
    D_700038A0 = *(float *)0x700038F0;
    D_700038A4 = D_700038F4 - 200.0f;
    *(float *)0x700038A8 = D_700038F8;
    *(float *)0x700038AC = 1.0f;
    if (func_0019A910(&D_700038F0, &D_700038A0, world) != 0
        && (*(short *)(D_700031D0 + 0x1A) & 0x7000)) {
        D_70003A38 = *(float *)0x700031B4;
        D_70003A38 += 2.0f;
    } else {
        D_70003A38 = *(float *)(self + 0x50) - 200.0f;
    }
    D_700038B0 = *(float *)0x700038F0;
    D_700038B4 = 200.0f + D_700038F4;
    *(float *)(0x700038B8) = D_700038F8;
    *(float *)0x700038BC = 1.0f;
    if (func_0019A910(&D_700038F0, &D_700038B0, world) != 0
        && (*(short *)(D_700031D0 + 0x1A) & 0x8800)) {
        h = *(float *)0x700031B4;
        *(float *)0x70003A3C = h;
        *(float *)0x70003A3C = h - 1.0f;
    } else if (D_00810700 == 0x12) {
        D_700038B0 = *(float *)(other + 0xB0);
        D_700038B4 = 200.0f + *(float *)(other + 0xB4);
        D_700038B8 = *(float *)(other + 0xB8);
        *(float *)0x700038BC = 1.0f;
        if (func_0019A910(other + 0xB0, &D_700038B0, world) != 0) {
            D_700038B0 = 0.0f;
            D_700038B4 = -1.0f;
            *(float *)(0x700038B8) = 0.0f;
            *(float *)0x700038BC = 1.0f;
            hit = D_700031D0;
            *(float *)0x700038F0 = *(float *)(hit + 0x24);
            D_700038F4 = *(float *)(hit + 0x28);
            D_700038F8 = *(float *)(hit + 0x2C);
            *(float *)0x700038FC = 1.0f;
            dot = func_00102738(&D_700038B0, &D_700038F0);
            *(float *)0x70003A3C = dot;
            if (!(dot <= 0.2f)) {
                h = *(float *)0x700031B4;
                *(float *)0x70003A3C = h;
                *(float *)0x70003A3C = h - 1.0f;
            } else {
                *(float *)0x70003A3C = 200.0f + *(float *)(self + 0x14);
            }
        }
    } else {
        *(float *)0x70003A3C = 200.0f + *(float *)(self + 0x14);
    }
    if (!(D_70003A38 <= *(float *)0x70003A3C)) {
        D_70003A38 = *(float *)0x70003A3C - 3.0f;
    }
    *(float *)(self + 0x50) = D_70003A38;
    *(float *)(self + 0x54) = *(float *)0x70003A3C;
    return flags;
}
