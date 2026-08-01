// func_001549C0 — byte-identical match (measured 100.0% with its declared CFLAGS).
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// SEMANTICS: ACTIVE tick of the kind-0xE tendril field. Sub-state +0x05 switch
// (0 SCAN / 1 DEPLOY / 2 HOLD / 3 RETRACT / 4 reset), then blend the tail RGB
// toward green and dispatch the renderer.
//
// SHAPE NOTES (all forced by the target, do not "clean up"):
//  * case 0 FALLS THROUGH into case 1 — once SCAN succeeds it advances the
//    sub-state and immediately runs one DEPLOY step in the same tick. The
//    func_00154460 guard is an early `break`, not an `if` wrapping the body:
//    a failed guard leaves the switch entirely, a passing one falls through.
//  * The random-scaling chains are written as compound assignments on ONE
//    variable (`s3 <<= 2; s3 >>= 15;` and `v0 *= 0x50; v0 >>= 15;`). That is
//    what gives the shift chain a home register instead of an expression temp,
//    reproducing the target's fresh-register colouring
//    (`sll v1,v0,4 / sra v1,v1,15 / addiu v0,v1,0x30`).
//  * The DEPLOY/RETRACT clamps store the incremented value, RELOAD it, clamp,
//    and store again — the target really does the redundant lh. The clamp temp
//    must be `int` (a `short` makes mwcc hoist the limit constant and
//    dsll32/dsra32 it every iteration), `b` must be declared before `i` (that
//    is what puts the pointer in $v1 and the counter in $a0), and the clamp
//    comparisons must be spelled `(t > 0x12C)` and `(0 > t)`: `(t < 0)` lets
//    mwcc collapse the test into a real `bgez`, while the constant-on-the-left
//    form keeps the target's `slt at,v0,zero / beqz at`.
//  * In case 2 the "arm the timer and advance" tail is written out in BOTH
//    arms of the distance test; the target emits three copies of it.
//  * The two trig products go into temps `t`/`u` and are stored only after
//    both calls; declaring `angle` before `radius` reproduces the target's
//    FP pair colouring ($f21 radius / $f20 angle).
//  * D_70003B8D must be spelled as a symbol (needs `D_70003B8D = 0x70003B8D;`
//    in config/SCUS_971.12.lcf, already present). build.normalize_asm()
//    symbolizes this function's .s per-file because the C references it.
extern volatile unsigned char D_70003B8D;           /* PS2 scratchpad @ 0x70003B8D */

extern float D_008102B0[];
extern short D_0026D320[];
extern unsigned char D_00248124[];
#define RNORM 4.656612873e-10f
extern int func_00154460(unsigned char *self);
extern int func_001545B0(unsigned char *self, float x, float z);
extern int func_00122BB8(void);
extern float func_0011DE90(float);
extern float func_0011E2A8(float);
extern float func_0011DF78(float);
extern void func_001FBD50(unsigned char *self, int a, int b, float f);
extern void func_001C6380(unsigned char *self);
extern void func_00154F00(unsigned char *self);

void func_001549C0(unsigned char *s7, unsigned char *s6) {
    float *plr = D_008102B0;
    int v0;

    switch (s7[5]) {
    case 0:
        if (func_00154460(s7) == 0) {
            break;
        }
        {
            int s1;
            int hit;
            int s3;
            unsigned char *s4;
            unsigned char *s5;
            *(float *)(s6 + 0x10) = plr[0x28];
            *(float *)(s6 + 0x14) = *(float *)(s7 + 0xB4);
            *(float *)(s6 + 0x18) = plr[0x2A];
            *(short *)(s7 + 0x28) = 8;
            s7[5] = s7[5] + 1;
            s3 = func_00122BB8() >> 16;
            s3 <<= 2;
            s3 >>= 15;
            hit = 0;
            s1 = 0;
            s4 = s6;
            s5 = s6;
            for (; s1 < 12; s1++) {
                float angle, radius, t, u;
                if (*(unsigned short *)(s7 + 0x2E) == 0) {
                    radius = 5.5f + 2.0f * ((float)func_00122BB8() * RNORM);
                } else {
                    radius = 7.0f + 2.5f * ((float)func_00122BB8() * RNORM);
                }
                angle = -3.1415927f + 6.2831855f * ((float)func_00122BB8() * RNORM);
                t = radius * func_0011DE90(angle);
                u = radius * func_0011E2A8(angle);
                *(float *)(s4 + 0x1C) = plr[0x28] + t;
                *(float *)(s4 + 0x20) = plr[0x2A] + u;
                if (func_001545B0(s7, *(float *)(s4 + 0x1C), *(float *)(s4 + 0x20)) == 0) {
                    *(short *)(s5 + 0x84) = 0;
                } else {
                    hit = 1;
                    *(short *)(s5 + 0x84) = hit;
                }
                v0 = func_00122BB8() >> 16;
                v0 *= 0x50;
                v0 >>= 15;
                *(short *)(s5 + 0x7C) = v0 + 0x30;
                *(short *)(s5 + 0x7E) = 0;
                *(short *)(s5 + 0x80) = 0;
                *(short *)(s5 + 0x82) = D_0026D320[s3 * 2];
                s3 = (s3 + 1) & 3;
                s4 += 8;
                s5 += 0xA;
            }
            if (hit != 0) {
                func_001FBD50(s7, 0x42D, 0, 300.0f);
            }
        }
        /* fallthrough */
    case 1:
    {
        unsigned char *b;
        int i = 0;
        short n;
        b = s6;
        for (; i < 12; i++) {
            int t;
            *(short *)(b + 0x80) = *(short *)(b + 0x80) + 0x25;
            t = *(short *)(b + 0x80);
            *(short *)(b + 0x80) = (t > 0x12C) ? 0x12C : t;
            b += 0xA;
        }
        n = *(short *)(s7 + 0x28) - 1;
        *(short *)(s7 + 0x28) = n;
        if (n <= 0) {
            s7[5] = s7[5] + 1;
        }
        break;
    }
    case 2:
        if (D_70003B8D != 0) {
            *(short *)(s7 + 0x28) = 8;
            s7[5] = s7[5] + 1;
        } else {
            float d = func_0011DF78(plr[0x29] - *(float *)(s7 + 0xB4));
            if (d > 3.0f + *(float *)(D_00248124 + s7[0xD] * 20)) {
                *(short *)(s7 + 0x28) = 8;
                s7[5] = s7[5] + 1;
            } else {
                float dx = plr[0x28] - *(float *)(s6 + 0x10);
                float dz = plr[0x2A] - *(float *)(s6 + 0x18);
                float sq = dx * dx + dz * dz;
                if (*(unsigned short *)(s7 + 0x2E) == 0) {
                    if (sq < 4.0f) {
                        break;
                    }
                } else {
                    if (sq < 16.0f) {
                        break;
                    }
                }
                *(short *)(s7 + 0x28) = 8;
                s7[5] = s7[5] + 1;
            }
        }
        break;
    case 3:
    {
        unsigned char *b;
        int i = 0;
        short n;
        b = s6;
        for (; i < 12; i++) {
            int t;
            *(short *)(b + 0x80) = *(short *)(b + 0x80) - 0x25;
            t = *(short *)(b + 0x80);
            *(short *)(b + 0x80) = (0 > t) ? 0 : t;
            b += 0xA;
        }
        n = *(short *)(s7 + 0x28) - 1;
        *(short *)(s7 + 0x28) = n;
        if (n <= 0) {
            s7[5] = s7[5] + 1;
        }
        break;
    }
    case 4:
        s7[5] = 0;
        break;
    }

    v0 = *(int *)(s7 + 0x20);
    if (v0 != 0) {
        float k = 1.0f - *(float *)(v0 + 0x80);
        *(float *)(s7 + 0x80) = (6.0f + k * (*(float *)(s6 + 0) - 6.0f)) / 128.0f;
        *(float *)(s7 + 0x84) = (92.0f + k * (*(float *)(s6 + 4) - 92.0f)) / 128.0f;
        *(float *)(s7 + 0x88) = (1.0f + k * (*(float *)(s6 + 8) - 1.0f)) / 128.0f;
    }
    func_001C6380(s7);
    if (s7[5] != 0) {
        func_00154F00(s7);
    }
}
