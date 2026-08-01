// NEARMISS func_001F4190  (vram 0x001F4190, ?) — readable decompilation, NOT byte-identical.
//
// objdiff 75.74% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FP register-coloring + list-scheduling wall across the whole particle loop (153 of 470 rows differ, 75.74%). The CFG, the jr-table dispatch (jtbl_0026EA00, all 6 case bodies byte-identical including the dead default `sw zero,0x3a20`), the frame (0x70 with f20/f21 saved), the prologue/epilogue, th...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// SEMANTICS: spawns one burst of `cfg->count` particles/debris pieces around an actor.
//   self = the emitting actor (self+0x100..0x108 = its world position,
//          self+0xD0 = its orientation basis, self[0xD] = its kind id).
//   arg1 = the timing block: +0x00 = the RNG seed for this burst, +0x04 = death tick,
//          +0x08 = current tick.  0x70003A20 receives (float)tick and life = death-tick.
//   cfg  = the emitter description:
//          +0x00/+0x04/+0x08 = position jitter radii (XYZ),
//          +0x10..+0x18 / +0x20..+0x28 = velocity scale / velocity bias per axis,
//          +0x30 = the piece template, +0x40/+0x44 = base+random uniform scale,
//          +0x48 = gravity/drift factor, +0x4C = piece count,
//          +0x50 / +0x54,+0x58 / +0x5C = the render ids handed to func_001F3E30
//          (the +0x54 pair alternates on even/odd piece index).
//   All randomness comes from one inline LCG, `rnd = rnd * 37 + 11`, whose top-16-bit
//   field is mapped to (0,1] as `((rnd>>16)&0xFFFF)/65535.0f + 1e-4f`; eight draws are
//   consumed per piece (2 for the launch direction, 3 for the velocity, 1 for the spin
//   axis angle, 1 for the scale, 1 for the per-kind spin rate).
//   self[0xD] selects the spin-rate multiplier via jtbl_0026EA00 (kinds 11..26); the
//   default arm clears 0x70003A20 (the tick scale) instead of 0x70003A24 - preserved
//   verbatim, it looks like an original bug.
//   0x700038A0/B0/C0/D0 are the EE-scratchpad staging vectors (position, velocity,
//   rotation quaternion, scale); 0x70003A20/A24 the two scalar scratch slots.

extern float func_0011DE90(float);
extern float func_0011E2A8(float);
extern void  func_001026A0(float *, unsigned char *, float *);
extern void  func_001028D0(float *, float *, unsigned char *);
extern void  func_001CA3B0(float *, float, float, float);
extern void  func_001CA4D0(float *, float *, float *);
extern void  quat_to_mat3(float *, float *, float *);
extern void  func_00102900(float *, float *, float);
extern void  func_001F3E30(float *, unsigned char *, int, int, int);

extern float D_700036A0[];
extern float D_700036B0[];
extern float D_700036C0[];
extern float D_700036E0[];
extern float D_700038A0[];
extern float D_700038B0[];
extern float D_700038C0[];

void func_001F4190(unsigned char *self, unsigned char *arg1, unsigned char *cfg)
{
    int rnd;
    int i;
    int life;
    int age;
    float ang;
    float r0;
    float r1;
    float r2;
    float r3;
    float r4;
    float r5;
    float r6;
    float r7;
    float a;
    float t;
    float t2;
    float sc;

    age = *(int *)(arg1 + 8);
    life = *(int *)(arg1 + 4) - age;
    rnd = *(int *)(arg1 + 0);
    i = 0;
    *(volatile float *)0x70003A20 = (float)age;
    while (i < *(int *)(cfg + 0x4C)) {
        r0 = (float)((rnd >> 16) & 0xFFFF) / 65535.0f + 9.99999975e-05f;
        rnd = rnd * 37 + 11;
        r1 = (float)((rnd >> 16) & 0xFFFF) / 65535.0f + 9.99999975e-05f;
        rnd = rnd * 37 + 11;
        ang = 6.28318548f * r0 - 3.14159274f;
        *(volatile float *)0x700038A0 = *(float *)(self + 0x100) + *(float *)cfg * func_0011DE90(ang);
        *(volatile float *)0x700038A4 = *(float *)(self + 0x104)
            + *(float *)(cfg + 4) * func_0011DE90(6.28318548f * r1 - 3.14159274f);
        *(volatile float *)0x700038A8 = *(float *)(self + 0x108) + *(float *)(cfg + 8) * func_0011E2A8(ang);
        *(volatile float *)0x700038AC = 1.0f;
        r2 = (float)((rnd >> 16) & 0xFFFF) / 65535.0f + 9.99999975e-05f;
        rnd = rnd * 37 + 11;
        r3 = (float)((rnd >> 16) & 0xFFFF) / 65535.0f + 9.99999975e-05f;
        rnd = rnd * 37 + 11;
        r4 = (float)((rnd >> 16) & 0xFFFF) / 65535.0f + 9.99999975e-05f;
        rnd = rnd * 37 + 11;
        *(volatile float *)0x700038B0 = *(float *)(cfg + 0x10) * r2 - *(float *)(cfg + 0x20);
        *(volatile float *)0x700038B4 = *(float *)(cfg + 0x14) * r3 - *(float *)(cfg + 0x24);
        *(volatile float *)0x700038B8 = *(float *)(cfg + 0x18) * r4 - *(float *)(cfg + 0x28);
        *(volatile float *)0x700038BC = 1.0f;
        func_001026A0(D_700038B0, self + 0xD0, D_700038B0);
        func_001028D0(D_700038B0, D_700038B0, self + 0x100);
        r5 = (float)((rnd >> 16) & 0xFFFF) / 65535.0f + 9.99999975e-05f;
        rnd = rnd * 37 + 11;
        t = 3.14159274f * (360.0f * r5) / 180.0f;
        func_001CA3B0(D_700038C0, t, t, t);
        r6 = (float)((rnd >> 16) & 0xFFFF) / 65535.0f + 9.99999975e-05f;
        rnd = rnd * 37 + 11;
        a = *(float *)(cfg + 0x40) + *(float *)(cfg + 0x44) * r6;
        *(volatile float *)0x700038D0 = a;
        *(volatile float *)0x700038D4 = a;
        *(volatile float *)0x700038D8 = a;
        *(volatile float *)0x700038DC = 1.0f;
        switch (self[0xD]) {
        case 21:
        case 24:
        case 25:
        case 26:
            *(volatile float *)0x70003A24 = 12.0f;
            break;
        case 15:
        case 16:
        case 19:
            *(volatile float *)0x70003A24 = 8.0f;
            break;
        case 11:
            *(volatile float *)0x70003A24 = 9.0f;
            break;
        case 12:
            *(volatile float *)0x70003A24 = 6.0f;
            break;
        case 13:
        case 14:
        case 20:
            *(volatile float *)0x70003A24 = 4.0f;
            break;
        default:
            *(volatile float *)0x70003A20 = 0.0f;
            break;
        }
        r7 = (float)((rnd >> 16) & 0xFFFF) / 65535.0f + 9.99999975e-05f;
        rnd = rnd * 37 + 11;
        t2 = 3.14159274f
            * (*(volatile float *)0x70003A20 * (*(volatile float *)0x70003A24 * r7)) / 180.0f;
        *(volatile float *)0x70003A24 = t2;
        func_001CA3B0(D_700036E0, t2, t2, t2);
        func_001CA4D0(D_700038C0, D_700038C0, D_700036E0);
        *(volatile float *)0x70003A24 = 1.0f;
        if (life < 20) {
            *(volatile float *)0x70003A24 = (float)life / 20.0f;
        }
        sc = *(volatile float *)0x70003A20;
        *(volatile float *)0x700038B4 = *(volatile float *)0x700038B4 + *(float *)(cfg + 0x48) * sc;
        *(volatile float *)0x700038A0 = *(volatile float *)0x700038A0 + *(volatile float *)0x700038B0 * sc;
        *(volatile float *)0x700038A4 = *(volatile float *)0x700038A4 + *(volatile float *)0x700038B4 * sc;
        *(volatile float *)0x700038A8 = *(volatile float *)0x700038A8 + *(volatile float *)0x700038B8 * sc;
        quat_to_mat3(D_700036A0, D_700038C0, D_700038A0);
        func_00102900(D_700036A0, D_700036A0, *(volatile float *)0x700038D0 * *(volatile float *)0x70003A24);
        func_00102900(D_700036B0, D_700036B0, *(volatile float *)0x700038D4 * *(volatile float *)0x70003A24);
        func_00102900(D_700036C0, D_700036C0, *(volatile float *)0x700038D8 * *(volatile float *)0x70003A24);
        func_001F3E30(D_700036A0, cfg + 0x30, *(int *)(cfg + 0x50),
                      *(int *)(cfg + (i % 2) * 4 + 0x54), *(int *)(cfg + 0x5C));
        i++;
    }
}
