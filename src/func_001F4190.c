// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 75.74% NEARMISS).
// Fixes read from the original instructions:
//  - Each random draw is spelled as the matched effect-driver twins do:
//    r = (float)value; r = r / 65535.0f; r += 0.0001f; then the seed advances.
//  - The launch angle is computed right after the first draw, before the
//    second draw.
//  - Each velocity component is stored right after its own draw (draw, store,
//    draw, store, ...), not after all three draws.
//  - The scratchpad vectors and the two scalar slots are relocated externs
//    (D_700038A0/B0/C0/D0, D_70003A20/A24), not volatile literal addresses.
//  - The three render ids are read as an int array: words 20, 21 + i % 2, 23.
//
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
extern float D_700038D0[];
extern float D_70003A20;
extern float D_70003A24;

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
    D_70003A20 = (float)age;
    while (i < *(int *)(cfg + 0x4C)) {
        r0 = (float)((rnd >> 16) & 0xFFFF);
        r0 = r0 / 65535.0f;
        r0 += 0.0001f;
        rnd = rnd * 37 + 11;
        ang = 6.28318548f * r0 - 3.14159274f;
        r1 = (float)((rnd >> 16) & 0xFFFF);
        r1 = r1 / 65535.0f;
        r1 += 0.0001f;
        rnd = rnd * 37 + 11;
        D_700038A0[0] = *(float *)(self + 0x100) + *(float *)cfg * func_0011DE90(ang);
        D_700038A0[1] = *(float *)(self + 0x104)
            + *(float *)(cfg + 4) * func_0011DE90(6.28318548f * r1 - 3.14159274f);
        D_700038A0[2] = *(float *)(self + 0x108) + *(float *)(cfg + 8) * func_0011E2A8(ang);
        D_700038A0[3] = 1.0f;
        r2 = (float)((rnd >> 16) & 0xFFFF);
        r2 = r2 / 65535.0f;
        r2 += 0.0001f;
        rnd = rnd * 37 + 11;
        D_700038B0[0] = *(float *)(cfg + 0x10) * r2 - *(float *)(cfg + 0x20);
        r3 = (float)((rnd >> 16) & 0xFFFF);
        r3 = r3 / 65535.0f;
        r3 += 0.0001f;
        rnd = rnd * 37 + 11;
        D_700038B0[1] = *(float *)(cfg + 0x14) * r3 - *(float *)(cfg + 0x24);
        r4 = (float)((rnd >> 16) & 0xFFFF);
        r4 = r4 / 65535.0f;
        r4 += 0.0001f;
        rnd = rnd * 37 + 11;
        D_700038B0[2] = *(float *)(cfg + 0x18) * r4 - *(float *)(cfg + 0x28);
        D_700038B0[3] = 1.0f;
        func_001026A0(D_700038B0, self + 0xD0, D_700038B0);
        func_001028D0(D_700038B0, D_700038B0, self + 0x100);
        r5 = (float)((rnd >> 16) & 0xFFFF);
        r5 = r5 / 65535.0f;
        r5 += 0.0001f;
        rnd = rnd * 37 + 11;
        t = 3.14159274f * (360.0f * r5) / 180.0f;
        func_001CA3B0(D_700038C0, t, t, t);
        r6 = (float)((rnd >> 16) & 0xFFFF);
        r6 = r6 / 65535.0f;
        r6 += 0.0001f;
        rnd = rnd * 37 + 11;
        a = *(float *)(cfg + 0x40) + *(float *)(cfg + 0x44) * r6;
        D_700038D0[0] = a;
        D_700038D0[1] = a;
        D_700038D0[2] = a;
        D_700038D0[3] = 1.0f;
        switch (self[0xD]) {
        case 21:
        case 24:
        case 25:
        case 26:
            D_70003A24 = 12.0f;
            break;
        case 15:
        case 16:
        case 19:
            D_70003A24 = 8.0f;
            break;
        case 11:
            D_70003A24 = 9.0f;
            break;
        case 12:
            D_70003A24 = 6.0f;
            break;
        case 13:
        case 14:
        case 20:
            D_70003A24 = 4.0f;
            break;
        default:
            D_70003A20 = 0.0f;
            break;
        }
        r7 = (float)((rnd >> 16) & 0xFFFF);
        r7 = r7 / 65535.0f;
        r7 += 0.0001f;
        rnd = rnd * 37 + 11;
        t2 = 3.14159274f
            * (D_70003A20 * (D_70003A24 * r7)) / 180.0f;
        D_70003A24 = t2;
        func_001CA3B0(D_700036E0, t2, t2, t2);
        func_001CA4D0(D_700038C0, D_700038C0, D_700036E0);
        D_70003A24 = 1.0f;
        if (life < 20) {
            D_70003A24 = (float)life / 20.0f;
        }
        sc = D_70003A20;
        D_700038B0[1] = D_700038B0[1] + *(float *)(cfg + 0x48) * sc;
        D_700038A0[0] = D_700038A0[0] + D_700038B0[0] * sc;
        D_700038A0[1] = D_700038A0[1] + D_700038B0[1] * sc;
        D_700038A0[2] = D_700038A0[2] + D_700038B0[2] * sc;
        quat_to_mat3(D_700036A0, D_700038C0, D_700038A0);
        func_00102900(D_700036A0, D_700036A0, D_700038D0[0] * D_70003A24);
        func_00102900(D_700036B0, D_700036B0, D_700038D0[1] * D_70003A24);
        func_00102900(D_700036C0, D_700036C0, D_700038D0[2] * D_70003A24);
        func_001F3E30(D_700036A0, cfg + 0x30, ((int *)cfg)[20],
                      ((int *)cfg)[21 + i % 2], ((int *)cfg)[23]);
        i++;
    }
}
