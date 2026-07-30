// NEARMISS func_0018F870  (vram 0x0018F870, 0x16A4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 94.15% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// 94.15% (mwcc233; 991202 = 89.47%). Logic/structure FULLY recovered, including the 0xC0 frame with the two 16-byte stack candidate buffers at sp+0xA0 / sp+0xB0 and the complete saved-register assignment (self=s6, other=s5, anim/hitA coalesced into s1, world=s4, hit0=s0, flags=s2, blocked=s3, hitB=...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

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
extern char D_700031B0[];
extern char D_700038A0[];
extern char D_700038B0[];
extern char D_700038C0[];
extern char D_700038D0[];
extern char D_700038E0[];
extern char D_700038F0[];
extern char D_70003900[];
extern char D_70003910[];
extern char D_70003920[];
extern char D_70003930[];
extern char D_70003940[];
extern char D_70003950[];
extern char D_70003960[];
extern char D_700039F0[];

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

    flags = 0;
    blocked = 0;
    func_00102948(D_700038B0, other + 0xB0);
    func_001028D0(D_700038A0, self + 0x10, D_700038B0);
    func_00102760(D_700038A0, D_700038A0);
    func_00103230(D_700038A0, D_700038A0, 1.5f);
    func_001028B8(D_700038A0, D_700038A0, self + 0x10);
    hit0 = func_0019A910(D_700038B0, D_700038A0, world);
    if (hit0 != 0) {
        flags = 1;
        hit = *(unsigned char **)0x700031D0;
        *(unsigned short *)(self + 0x58) = *(unsigned short *)(hit + 0x1A);
        func_001031E0(D_700038C0, D_700031B0);
        func_00102948(D_70003950, D_700038C0);
        func_001028D0(D_700038F0, self + 0x20, self + 0x10);
        *(volatile float *)0x700038F4 = 0.0f;
        func_00102760(D_700038F0, D_700038F0);
        func_001028D0(D_70003960, other + 0xB0, self + 0x10);
        *(volatile float *)0x70003964 = 0.0f;
        func_00102760(D_70003960, D_70003960);
        hit = *(unsigned char **)0x700031D0;
        *(volatile float *)0x700038E0 = *(float *)(hit + 0x24);
        *(volatile float *)0x700038E8 = *(float *)(hit + 0x2C);
        *(volatile float *)0x700038E4 = 0.0f;
        *(volatile float *)0x700038EC = 1.0f;
        dot = func_00102738(D_700038F0, D_700038E0);
        *(volatile float *)0x70003A3C = dot;
        if (dot < 0.99f) {
            blocked = 1;
        }
        *(volatile float *)0x700038E4 = *(float *)(*(unsigned char **)0x700031D0 + 0x28);
        func_001028D0(D_700038A0, D_700038A0, D_700038B0);
        func_00102760(D_700038A0, D_700038A0);
        sflags = *(unsigned short *)(self + 0x58);
        if (sflags & 0xD800) {
            if (sflags & 0x8800) {
                h = *(volatile float *)0x700038C4;
                flags = 8;
                if (h < *(float *)(self + 0x50)) {
                    *(float *)(self + 0x50) = h;
                }
            } else {
                h = *(volatile float *)0x700038C4;
                flags = 0x10;
                if (!(h <= *(float *)(self + 0x54))) {
                    *(float *)(self + 0x54) = h;
                }
            }
            func_00102948(self + 0x10, D_700038C0);
            *(float *)(self + 0x10) = *(float *)(self + 0x10)
                + 0.5f * *(volatile float *)0x700038A0;
            *(float *)(self + 0x18) = *(float *)(self + 0x18)
                + 0.5f * *(volatile float *)0x700038A8;
        } else {
            *(float *)(self + 0x10) = *(volatile float *)0x700038C0;
            *(float *)(self + 0x18) = *(volatile float *)0x700038C8;
            *(float *)(self + 0x10) = *(float *)(self + 0x10)
                + 0.5f * *(volatile float *)0x700038A0;
            *(float *)(self + 0x18) = *(float *)(self + 0x18)
                + 0.5f * *(volatile float *)0x700038A8;
            if (hit0 == 4) {
                func_00102948(D_700038D0, D_700038C0);
                func_001028D0(D_700038D0, D_700038D0, D_700038A0);
                func_0018CE60(self, D_700038D0, anim);
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
    } else {
        mode = 1;
        if (blocked != 1) {
            mode = 2;
            if (hit0 == 0) {
                mode = 1;
            }
        }
    }
    if (mode == 2) {
        ang = func_001B1470(func_0011E620(*(volatile float *)0x700038E0,
                                          *(volatile float *)0x700038E8) - 1.5707964f);
        func_001028B8(D_700039F0, self + 0x10, D_700038E0);
        *(volatile float *)0x700038A0 = 5.5f * func_0011E2A8(ang);
        *(volatile float *)0x700038A8 = 5.5f * func_0011DE90(ang);
        *(volatile float *)0x700038A4 = 0.0f;
        func_001028B8(D_700038B0, D_700039F0, D_700038A0);
        func_00102760(D_70003910, D_700038A0);
        func_00103230(D_70003910, D_70003910, -3.0f);
        func_001028B8(D_70003910, D_700039F0, D_70003910);
        if (func_0019A910(D_70003910, D_700038B0, world) != 0) {
            hit = *(unsigned char **)0x700031D0;
            if (*(short *)(hit + 0x1A) & 0x2000) {
                *(volatile float *)0x70003900 = *(float *)(hit + 0x24);
                *(volatile float *)0x70003904 = *(float *)(hit + 0x28);
                *(volatile float *)0x70003908 = *(float *)(hit + 0x2C);
                *(volatile float *)0x7000390C = 1.0f;
                dot = func_00102738(D_70003900, D_700038E0);
                *(volatile float *)0x70003A3C = dot;
                if (dot < 0.9f) {
                    func_001028D0(D_700038C0, D_700031B0, D_700038A0);
                    flags |= 2;
                    *(float *)(self + 0x10) = *(volatile float *)0x700038C0;
                    *(float *)(self + 0x18) = *(volatile float *)0x700038C8;
                }
            }
        }
        if ((flags & 2) == 0) {
            func_001028D0(D_700038B0, D_700039F0, D_700038A0);
            func_00102760(D_70003910, D_700038A0);
            func_00103230(D_70003910, D_70003910, 3.0f);
            func_001028B8(D_70003910, D_700039F0, D_70003910);
            if (func_0019A910(D_70003910, D_700038B0, world) != 0) {
                hit = *(unsigned char **)0x700031D0;
                if (*(short *)(hit + 0x1A) & 0x2000) {
                    *(volatile float *)0x70003900 = *(float *)(hit + 0x24);
                    *(volatile float *)0x70003904 = *(float *)(hit + 0x28);
                    *(volatile float *)0x70003908 = *(float *)(hit + 0x2C);
                    *(volatile float *)0x7000390C = 1.0f;
                    dot = func_00102738(D_70003900, D_700038E0);
                    *(volatile float *)0x70003A3C = dot;
                    if (dot < 0.9f) {
                        func_001028B8(D_700038C0, D_700031B0, D_700038A0);
                        flags |= 4;
                        *(float *)(self + 0x10) = *(volatile float *)0x700038C0;
                        *(float *)(self + 0x18) = *(volatile float *)0x700038C8;
                    }
                }
            }
        }
    } else if (mode == 1) {
        ang = func_001B1470(func_001B1240(self + 0x10, *(float *)(self + 0x20),
                                          *(float *)(self + 0x28)) - 1.5707964f);
        if (hit0 == 0) {
            *(volatile float *)0x700038A0 = 5.5f * func_0011E2A8(ang);
            *(volatile float *)0x700038A8 = 5.5f * func_0011DE90(ang);
            *(volatile float *)0x700038A4 = 0.0f;
            func_001028B8(D_700038B0, self + 0x10, D_700038A0);
            func_00102948(D_70003910, self + 0x10);
        } else {
            *(volatile float *)0x700038A0 = 5.5f * func_0011E2A8(ang);
            *(volatile float *)0x700038A8 = 5.5f * func_0011DE90(ang);
            *(volatile float *)0x700038A4 = 0.0f;
            func_001028B8(D_700038B0, self + 0x10, D_700038A0);
            func_00102760(D_70003910, D_700038A0);
            func_00103230(D_70003910, D_70003910, -3.0f);
            func_001028B8(D_70003910, self + 0x10, D_70003910);
        }
        func_00102948(bufA, self + 0x10);
        func_00102948(bufB, self + 0x10);
        hitA = func_0019A910(D_70003910, D_700038B0, world);
        if (hitA != 0) {
            func_00102948(D_70003920, D_700031B0);
            hit = *(unsigned char **)0x700031D0;
            *(volatile float *)0x70003900 = *(float *)(hit + 0x24);
            *(volatile float *)0x70003904 = *(float *)(hit + 0x28);
            *(volatile float *)0x70003908 = *(float *)(hit + 0x2C);
            *(volatile float *)0x7000390C = 1.0f;
            if (hit0 != 0) {
                if (flags & 8) {
                    dot = func_00102738(D_70003900, D_70003960);
                    *(volatile float *)0x70003A3C = dot;
                    if (dot < -0.08f) {
                        hitA = 0;
                        *(volatile float *)0x70003A3C = -1.0f;
                    }
                } else {
                    dot = func_00102738(D_70003900, D_700038E0);
                    *(volatile float *)0x70003A3C = dot;
                    if (dot < -0.998f) {
                        hitA = 0;
                        *(volatile float *)0x70003A3C = -1.0f;
                    }
                }
            } else {
                *(volatile float *)0x70003A3C = 0.0f;
            }
            if (hitA != 0) {
                h = *(volatile float *)0x70003A3C;
                if (blocked == 1 || (h < 0.9f && !(h <= -0.3f))) {
                    sflags = *(unsigned short *)(*(unsigned char **)0x700031D0 + 0x1A);
                    func_001028D0(D_700038D0, D_700038B0, D_70003910);
                    func_001028D0(D_700038C0, D_700031B0, D_700038A0);
                    func_00102760(D_700038D0, D_700038D0);
                    if (sflags & 0x8800) {
                        *(volatile float *)0x700038F0 = 0.0f;
                        *(volatile float *)0x700038F4 = -1.0f;
                        *(volatile float *)0x700038F8 = 0.0f;
                        *(volatile float *)0x700038FC = 1.0f;
                        dot = func_00102738(D_700038F0, D_70003900);
                        *(volatile float *)0x70003A3C = dot;
                        if (dot < 0.9f) {
                            flags |= 2;
                            bufA[0] = *(volatile float *)0x700038C0;
                            bufA[2] = *(volatile float *)0x700038C8;
                            bufA[0] += 0.1f * *(volatile float *)0x700038D0;
                            bufA[2] += 0.1f * *(volatile float *)0x700038D8;
                        } else {
                            func_00102948(bufA, D_700038C0);
                            flags |= 0xA;
                            bufA[0] += 0.1f * *(volatile float *)0x700038D0;
                            bufA[2] += 0.1f * *(volatile float *)0x700038D8;
                        }
                    } else if (sflags & 0x2000) {
                        flags |= 2;
                        bufA[0] = *(volatile float *)0x700038C0;
                        bufA[2] = *(volatile float *)0x700038C8;
                        bufA[0] += 0.1f * *(volatile float *)0x700038D0;
                        bufA[2] += 0.1f * *(volatile float *)0x700038D8;
                    } else {
                        func_00102948(bufA, D_700038C0);
                        flags |= 3;
                        bufA[0] += 0.1f * *(volatile float *)0x700038D0;
                        bufA[2] += 0.1f * *(volatile float *)0x700038D8;
                    }
                }
            }
        }
        if (hit0 == 0) {
            func_001028D0(D_700038B0, self + 0x10, D_700038A0);
            func_00102948(D_70003910, self + 0x10);
        } else {
            func_001028D0(D_700038B0, self + 0x10, D_700038A0);
            func_00102760(D_70003910, D_700038A0);
            func_00103230(D_70003910, D_70003910, 3.0f);
            func_001028B8(D_70003910, self + 0x10, D_70003910);
        }
        hitB = func_0019A910(D_70003910, D_700038B0, world);
        if (hitB != 0) {
            func_00102948(D_70003930, D_700031B0);
            hit = *(unsigned char **)0x700031D0;
            *(volatile float *)0x700038F0 = *(float *)(hit + 0x24);
            *(volatile float *)0x700038F4 = *(float *)(hit + 0x28);
            *(volatile float *)0x700038F8 = *(float *)(hit + 0x2C);
            *(volatile float *)0x700038FC = 1.0f;
            if (hit0 != 0) {
                if (flags & 8) {
                    dot = func_00102738(D_700038F0, D_70003960);
                    *(volatile float *)0x70003A3C = dot;
                    if (dot < -0.08f) {
                        blocked = 0;
                        hitB = 0;
                        *(volatile float *)0x70003A3C = -1.0f;
                    }
                } else {
                    dot = func_00102738(D_700038F0, D_700038E0);
                    *(volatile float *)0x70003A3C = dot;
                    if (dot < -0.998f) {
                        blocked = 0;
                        hitB = 0;
                        *(volatile float *)0x70003A3C = -1.0f;
                    }
                }
            } else {
                *(volatile float *)0x70003A3C = 0.0f;
            }
            h = *(volatile float *)0x70003A3C;
            if (blocked == 1 || (h < 0.9f && !(h <= -0.3f))) {
                sflags = *(unsigned short *)(*(unsigned char **)0x700031D0 + 0x1A);
                func_001028D0(D_700038D0, D_700038B0, D_70003910);
                func_001028B8(D_700038C0, D_700031B0, D_700038A0);
                func_00102760(D_700038D0, D_700038D0);
                if (sflags & 0x8800) {
                    *(volatile float *)0x700038B0 = 0.0f;
                    *(volatile float *)0x700038B4 = -1.0f;
                    *(volatile float *)0x700038B8 = 0.0f;
                    *(volatile float *)0x700038BC = 1.0f;
                    dot = func_00102738(D_700038B0, D_700038F0);
                    *(volatile float *)0x70003A3C = dot;
                    if (dot < 0.9f) {
                        flags |= 4;
                        bufB[0] = *(volatile float *)0x700038C0;
                        bufB[2] = *(volatile float *)0x700038C8;
                        bufB[0] += 0.1f * *(volatile float *)0x700038D0;
                        bufB[2] += 0.1f * *(volatile float *)0x700038D8;
                    } else {
                        func_00102948(bufB, D_700038C0);
                        flags |= 8;
                        bufB[0] += 0.1f * *(volatile float *)0x700038D0;
                        bufB[2] += 0.1f * *(volatile float *)0x700038D8;
                    }
                } else if (sflags & 0x2000) {
                    flags |= 4;
                    bufB[0] = *(volatile float *)0x700038C0;
                    bufB[2] = *(volatile float *)0x700038C8;
                    bufB[0] += 0.1f * *(volatile float *)0x700038D0;
                    bufB[2] += 0.1f * *(volatile float *)0x700038D8;
                } else {
                    func_00102948(bufB, D_700038C0);
                    flags |= 1;
                    bufB[0] += 0.1f * *(volatile float *)0x700038D0;
                    bufB[2] += 0.1f * *(volatile float *)0x700038D8;
                }
            }
        }
        if ((flags & 6) == 6) {
            func_001028B8(D_70003940, D_70003920, D_70003930);
            func_00103230(D_70003940, D_70003940, 0.5f);
            *(float *)(self + 0x10) = *(volatile float *)0x70003940;
            *(float *)(self + 0x18) = *(volatile float *)0x70003948;
        } else if (flags & 2) {
            if (hitB != 0) {
                func_001028B8(D_70003940, D_70003920, D_70003930);
                func_00103230(D_70003940, D_70003940, 0.5f);
                *(float *)(self + 0x10) = *(volatile float *)0x70003940;
                *(float *)(self + 0x18) = *(volatile float *)0x70003948;
            } else {
                func_00102948(self + 0x10, bufA);
            }
        } else if (flags & 4) {
            if (hitA != 0) {
                func_001028B8(D_70003940, D_70003920, D_70003930);
                func_00103230(D_70003940, D_70003940, 0.5f);
                *(float *)(self + 0x10) = *(volatile float *)0x70003940;
                *(float *)(self + 0x18) = *(volatile float *)0x70003948;
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
                if (!(*(float *)(self + 0x14) <= *(volatile float *)0x70003954 - 1.0f)) {
                    *(float *)(self + 0x14) = *(volatile float *)0x70003954 - 1.0f;
                }
            } else {
                if (*(float *)(self + 0x14) < 1.0f + *(volatile float *)0x70003954) {
                    *(float *)(self + 0x14) = 1.0f + *(volatile float *)0x70003954;
                }
            }
        }
    }
    if ((flags & 0x1F) != 0) {
        func_00102948(D_700038B0, other + 0xB0);
        func_00102948(D_700038A0, self + 0x10);
        if (func_0019A910(D_700038B0, self + 0x10, world) != 0) {
            func_00102948(D_700038C0, D_700031B0);
            *(float *)(self + 0x10) = *(volatile float *)0x700038C0;
            *(float *)(self + 0x18) = *(volatile float *)0x700038C8;
        }
    }
    func_00102948(D_700038F0, other + 0xA0);
    *(volatile float *)0x700038F4 = *(volatile float *)0x700038F4 + 11.0f;
    func_001028D0(D_700038F0, self + 0x10, D_700038F0);
    func_00102760(D_700038F0, D_700038F0);
    func_001028D0(D_700038F0, self + 0x10, D_700038F0);
    *(volatile float *)0x700038A0 = *(volatile float *)0x700038F0;
    *(volatile float *)0x700038A4 = *(volatile float *)0x700038F4 - 200.0f;
    *(volatile float *)0x700038A8 = *(volatile float *)0x700038F8;
    *(volatile float *)0x700038AC = 1.0f;
    if (func_0019A910(D_700038F0, D_700038A0, world) != 0
        && (*(short *)(*(unsigned char **)0x700031D0 + 0x1A) & 0x7000)) {
        *(volatile float *)0x70003A38 = *(volatile float *)0x700031B4;
        *(volatile float *)0x70003A38 = *(volatile float *)0x700031B4 + 2.0f;
    } else {
        *(volatile float *)0x70003A38 = *(float *)(self + 0x50) - 200.0f;
    }
    *(volatile float *)0x700038B0 = *(volatile float *)0x700038F0;
    *(volatile float *)0x700038B4 = 200.0f + *(volatile float *)0x700038F4;
    *(volatile float *)0x700038B8 = *(volatile float *)0x700038F8;
    *(volatile float *)0x700038BC = 1.0f;
    if (func_0019A910(D_700038F0, D_700038B0, world) != 0
        && (*(short *)(*(unsigned char **)0x700031D0 + 0x1A) & 0x8800)) {
        *(volatile float *)0x70003A3C = *(volatile float *)0x700031B4;
        *(volatile float *)0x70003A3C = *(volatile float *)0x700031B4 - 1.0f;
    } else if (D_00810700 == 0x12) {
        *(volatile float *)0x700038B0 = *(float *)(other + 0xB0);
        *(volatile float *)0x700038B4 = 200.0f + *(float *)(other + 0xB4);
        *(volatile float *)0x700038B8 = *(float *)(other + 0xB8);
        *(volatile float *)0x700038BC = 1.0f;
        if (func_0019A910(other + 0xB0, D_700038B0, world) != 0) {
            *(volatile float *)0x700038B0 = 0.0f;
            *(volatile float *)0x700038B4 = -1.0f;
            *(volatile float *)0x700038B8 = 0.0f;
            *(volatile float *)0x700038BC = 1.0f;
            hit = *(unsigned char **)0x700031D0;
            *(volatile float *)0x700038F0 = *(float *)(hit + 0x24);
            *(volatile float *)0x700038F4 = *(float *)(hit + 0x28);
            *(volatile float *)0x700038F8 = *(float *)(hit + 0x2C);
            *(volatile float *)0x700038FC = 1.0f;
            dot = func_00102738(D_700038B0, D_700038F0);
            *(volatile float *)0x70003A3C = dot;
            if (!(dot <= 0.2f)) {
                *(volatile float *)0x70003A3C = *(volatile float *)0x700031B4;
                *(volatile float *)0x70003A3C = *(volatile float *)0x700031B4 - 1.0f;
            } else {
                *(volatile float *)0x70003A3C = 200.0f + *(float *)(self + 0x14);
            }
        }
    } else {
        *(volatile float *)0x70003A3C = 200.0f + *(float *)(self + 0x14);
    }
    h = *(volatile float *)0x70003A3C;
    if (!(*(volatile float *)0x70003A38 <= h)) {
        *(volatile float *)0x70003A38 = h - 3.0f;
    }
    *(float *)(self + 0x50) = *(volatile float *)0x70003A38;
    *(float *)(self + 0x54) = *(volatile float *)0x70003A3C;
    return flags;
}
