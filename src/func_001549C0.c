// NEARMISS func_001549C0  (vram 0x001549C0, 0x540 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 63.95% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Callee-saved register-allocation permutation. The target colors the SCAN loop counter -> $s1 and the girth/seed index -> $s3; mwcc swaps them ($s1=index, $s3=counter). The off-by-one cascades through every $s1/$s3 use in the 12-iteration scatter loop plus a paired FP-coloring shift ($f1/$f2 vs $f...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// NEARMISS 63.9% (mwcc 2.3.3). Logic fully recovered (verified instruction
// by instruction); residual is a callee-saved register-allocation
// permutation: the target colors the SCAN loop counter -> $s1 and the girth
// index -> $s3, while mwcc swaps them ($s1=index, $s3=counter). That off-by-
// one cascades through every $s1/$s3 reference plus the matching FP-coloring
// shift ($f1/$f2 vs $f4/$f5). Permuter territory, not the clean-store nop.
//
// SEMANTICS (resolved s33): ACTIVE tick of the kind-0xE tendril field.
// Sub-state +0x05 switch (0 SCAN / 1 DEPLOY / 2 HOLD / 3 RETRACT / 4 reset),
// then blend the tail RGB toward green and dispatch the renderer.
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

int func_001549C0(unsigned char *s7, unsigned char *s6) {
    float *plr = D_008102B0;
    int v0;

    switch (s7[5]) {
    case 0:
        if (func_00154460(s7) != 0) {
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
            s3 = ((func_00122BB8() >> 16) << 2) >> 15;
            hit = 0;
            s4 = s6;
            s5 = s6;
            for (s1 = 0; s1 < 12; s1++) {
                float radius, angle, t;
                if (*(unsigned short *)(s7 + 0x2E) == 0) {
                    radius = 5.5f + 2.0f * ((float)func_00122BB8() * RNORM);
                } else {
                    radius = 7.0f + 2.5f * ((float)func_00122BB8() * RNORM);
                }
                angle = -3.1415927f + 6.2831855f * ((float)func_00122BB8() * RNORM);
                t = radius * func_0011DE90(angle);
                *(float *)(s4 + 0x1C) = plr[0x28] + t;
                *(float *)(s4 + 0x20) = plr[0x2A] + radius * func_0011E2A8(angle);
                if (func_001545B0(s7, *(float *)(s4 + 0x1C), *(float *)(s4 + 0x20)) != 0) {
                    hit = 1;
                    *(short *)(s5 + 0x84) = hit;
                } else {
                    *(short *)(s5 + 0x84) = 0;
                }
                v0 = func_00122BB8();
                *(short *)(s5 + 0x7C) = ((((v0 >> 16) * 5) << 4) >> 15) + 0x30;
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
        break;
    case 1:
    {
        int i;
        unsigned char *b = s6;
        for (i = 0; i < 12; i++) {
            short t = *(short *)(b + 0x80) + 0x25;
            if (t >= 0x12D) {
                t = 0x12C;
            }
            *(short *)(b + 0x80) = t;
            b += 0xA;
        }
        *(short *)(s7 + 0x28) = *(short *)(s7 + 0x28) - 1;
        if ((short)*(short *)(s7 + 0x28) <= 0) {
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
            if (d <= 3.0f + *(float *)(D_00248124 + s7[0xD] * 20)) {
                float dx = plr[0x28] - *(float *)(s6 + 0x10);
                float dz = plr[0x2A] - *(float *)(s6 + 0x18);
                float sq = dx * dx + dz * dz;
                if (*(unsigned short *)(s7 + 0x2E) != 0) {
                    if (sq < 16.0f) {
                        break;
                    }
                } else {
                    if (sq < 4.0f) {
                        break;
                    }
                }
            }
            *(short *)(s7 + 0x28) = 8;
            s7[5] = s7[5] + 1;
        }
        break;
    case 3:
    {
        int i;
        unsigned char *b = s6;
        for (i = 0; i < 12; i++) {
            short t = *(short *)(b + 0x80) - 0x25;
            if (t < 0) {
                t = 0;
            }
            *(short *)(b + 0x80) = t;
            b += 0xA;
        }
        *(short *)(s7 + 0x28) = *(short *)(s7 + 0x28) - 1;
        if ((short)*(short *)(s7 + 0x28) <= 0) {
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
    return 0;
}
