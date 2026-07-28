// NEARMISS func_00230230  (vram 0x00230230, 0x5B4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.79% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Regalloc/scheduling permutation class (mwcc 2.3.3), NOT the clean-store nop. Body byte-identical except 28 bytes in 3 regions, each proven source-shape-invariant: (a) +0x27C c.eq.s operand order for f!=0.0f — 2.3.3 sorts commutative FP-compare operands (emits c.eq.s f0,f1; target f1,f0); f!=0 / 0...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Camera/aim angle update driven by the target entity's state (*(other+0x230)).
// Gates on func_00191350(other); computes a clamped pitch offset into self+0x94
// from the target height (other+0xA4), then per-state:
//  20: chase target yaw (other+0x218)+pi at 1 deg/tick, orbit at radius -35
//  21: only re-drive the follow distance (func_00191D40)
//  10/25: steer toward other+0xC4 (+/- 35 or 40 deg lead by sign of other+0x38,
//         or func_00191120 homing when other+0x38 == 0), orbit at radius
//         (self+0xC + self+0x94), clamp self+0x10/self+0x18 to 447/1340,
//         drive follow distance via func_00192010
// All handled states end by smoothing the self+0x10 vector into D_008105D0
// (rate 0.8) and calling func_0018D7B0(self, 4). Returns 1 if a state was
// handled, 0 otherwise.

extern int func_00191350(unsigned char *);
extern float func_0011DF78(float);
extern float func_0011E2A8(float);                  /* sin */
extern float func_0011DE90(float);                  /* cos */
extern float func_001B1470(float);                  /* angle wrap */
extern float func_001B12B0(float, float, float);    /* chase angle at rate */
extern float func_00191120(float, float, float, float);
extern void func_00191D40(unsigned char *, float, float);
extern void func_00192010(unsigned char *, float, float, float);
extern void func_0018D7B0(unsigned char *, int);
extern void func_0018C6A0(void *, void *, float);
extern void func_0018C4B0(void *, float, float);

extern float D_008105D0;
extern float D_008105E0;
extern float D_008105E8;

int func_00230230(unsigned char *self, unsigned char *other)
{
    float v;
    float w;
    int st;

    if (func_00191350(other)) {
        v = func_0011DF78(555.4f - *(float *)(other + 0xA4));
        if (v > 123.399994f) {
            v = 123.399994f;
        }
        w = 123.399994f - v;
        if (w > 90.0f) {
            w = 90.0f;
        }
        *(float *)(self + 0x94) = -w;

        st = *(int *)(other + 0x230);
        switch (st) {
        case 20:
            {
                float r;

                r = func_001B12B0(func_001B1470(3.1415927f + *(float *)(other + 0x218)),
                                  *(float *)(self + 0x44), 0.017453292f);
                *(float *)(self + 0x44) = r;
                *(float *)(self + 0x10) = D_008105E0 + -35.0f * func_0011E2A8(r);
                *(float *)(self + 0x18) = D_008105E8 + -35.0f * func_0011DE90(*(float *)(self + 0x44));
                func_00191D40(self, 60.0f + (*(float *)(self + 0x8C) + (11.0f + (*(float *)(self + 0x5C) + *(float *)(other + 0xA4)))), 0.8f);
                func_0018D7B0(self, 4);
                func_0018C6A0(self + 0x10, &D_008105D0, 0.8f);
                func_0018C4B0(&D_008105D0, *(float *)(self + 0x14), 0.8f);
                return 1;
            }
        case 21:
            func_00191D40(self, 60.0f + (*(float *)(self + 0x8C) + (11.0f + (*(float *)(self + 0x5C) + *(float *)(other + 0xA4)))), 0.8f);
            func_0018D7B0(self, 4);
            func_0018C6A0(self + 0x10, &D_008105D0, 0.8f);
            func_0018C4B0(&D_008105D0, *(float *)(self + 0x14), 0.8f);
            return 1;
        case 25:
        case 10:
            {
                float f;

                f = *(float *)(other + 0x38);
                if (f != 0.0f) {
                    if (st == 21) {     /* never true here (st is 10/25) — kept from original */
                        if (f >= 0.0f) {
                            *(float *)(self + 0x44) = func_001B12B0(func_001B1470((3.1415927f + *(float *)(other + 0xC4)) - 0.69813174f),
                                                                    *(float *)(self + 0x44), 0.005235988f);
                        } else {
                            *(float *)(self + 0x44) = func_001B12B0(func_001B1470(0.69813174f + (3.1415927f + *(float *)(other + 0xC4))),
                                                                    *(float *)(self + 0x44), 0.005235988f);
                        }
                    } else {
                        if (f >= 0.0f) {
                            *(float *)(self + 0x44) = func_001B12B0(func_001B1470(0.6108653f + *(float *)(other + 0xC4)),
                                                                    *(float *)(self + 0x44), 0.005235988f);
                        } else {
                            *(float *)(self + 0x44) = func_001B12B0(func_001B1470(*(float *)(other + 0xC4) - 0.6108653f),
                                                                    *(float *)(self + 0x44), 0.005235988f);
                        }
                    }
                    *(float *)(self + 0x10) = D_008105E0 + (*(float *)(self + 0xC) + *(float *)(self + 0x94)) * func_0011E2A8(*(float *)(self + 0x44));
                    *(float *)(self + 0x18) = D_008105E8 + (*(float *)(self + 0xC) + *(float *)(self + 0x94)) * func_0011DE90(*(float *)(self + 0x44));
                } else {
                    float r;
                    float s;

                    if (st == 21) {     /* never true here — kept from original */
                        r = func_00191120(3.1415927f + *(float *)(other + 0xC4), *(float *)(self + 0x44), 0.034906585f, 0.7853982f);
                    } else {
                        r = func_00191120(*(float *)(other + 0xC4), *(float *)(self + 0x44), 0.034906585f, 0.7853982f);
                    }
                    *(float *)(self + 0x44) = r;
                    /* s-temp keeps mwcc from hoisting the radius sum across the
                       sin call into a callee-saved reg (fs0 spill, frame 0x40) */
                    s = func_0011E2A8(r);
                    *(float *)(self + 0x10) = D_008105E0 + (*(float *)(self + 0xC) + *(float *)(self + 0x94)) * s;
                    *(float *)(self + 0x18) = D_008105E8 + (*(float *)(self + 0xC) + *(float *)(self + 0x94)) * func_0011DE90(*(float *)(self + 0x44));
                }
                func_00192010(self, *(float *)(self + 0x8C) + (*(float *)(self + 0x5C) + *(float *)(other + 0xB4)), 35.0f, 30.0f);
                if (*(float *)(self + 0x10) >= 447.0f) {
                    *(float *)(self + 0x10) = 447.0f;
                    if (*(float *)(self + 0x18) < 1340.0f) {
                        *(float *)(self + 0x18) = 1340.0f;
                    }
                }
                func_0018D7B0(self, 4);
                func_0018C6A0(self + 0x10, &D_008105D0, 0.8f);
                func_0018C4B0(&D_008105D0, *(float *)(self + 0x14), 0.8f);
                return 1;
            }
        default:
            return 0;
        }
    }
    return 0;
}
