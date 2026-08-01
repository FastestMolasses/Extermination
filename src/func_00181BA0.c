// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// SEMANTICS: per-frame "advance along facing" step for an actor, with the
// forward step distance chosen from a 14-entry jump table on the actor's
// current motion/animation id.
//   e[+0x20C] (s16)  = motion id; only 0xBC..0xC9 do anything (else both
//                      speeds keep their previous scratchpad values)
//   e[+0xD0]         = the actor's 4x4 orientation matrix
//   e[+0x2F4],+0x2F8 = base (pre-step) world x/z for the body position
//   e[+0xB0], +0xB8  = body world position x/z  (written, not accumulated)
//   e[+0x290],+0x298 = secondary world position x/z (accumulated)
// Scratchpad (SPR) working storage:
//   0x70003A20 = forward step distance for the body position
//   0x70003A24 = forward step distance for the secondary position
//   0x700038A0..AC = the local-space input vector (0, 0, step, 0)
//   0x700038B0..BC = the rotated output vector from func_001026A0
// The motion id repeats with period 7 (0xBC..0xC2 and 0xC3..0xC9 select the
// same pair of speeds), which is why the 14-entry table has only 7 distinct
// targets. Speeds used: 2.25, 4.5, 6.75, 9.0 (and 0.0 for the two ids that
// suppress the secondary step).
// Both stores of the step distance go through raw SPR addresses (mwcc folds a
// float constant store into lui+sw of the bit pattern), while the two vectors
// are passed to func_001026A0 by symbol -- that split reproduces the target's
// mix of relocated and non-relocated 0x7000xxxx references exactly.
//

extern void func_001026A0(float *, float *, float *);

extern float D_700038A0[4];
extern float D_700038B0[4];

void func_00181BA0(unsigned char *e)
{
    switch (*(short *)(e + 0x20C)) {
    case 0xBC:
    case 0xC3:
        *(float *)0x70003A20 = 2.25f;
        *(float *)0x70003A24 = 4.5f;
        break;
    case 0xC1:
    case 0xC8:
        *(float *)0x70003A24 = 0.0f;
        *(float *)0x70003A20 = 2.25f;
        break;
    case 0xC2:
    case 0xC9:
        *(float *)0x70003A24 = 0.0f;
        *(float *)0x70003A20 = 4.5f;
        break;
    case 0xBE:
    case 0xC4:
        *(float *)0x70003A20 = 4.5f;
        *(float *)0x70003A24 = 4.5f;
        break;
    case 0xC0:
    case 0xC6:
        *(float *)0x70003A20 = 6.75f;
        *(float *)0x70003A24 = 9.0f;
        break;
    case 0xBD:
    case 0xC5:
        *(float *)0x70003A20 = 6.75f;
        *(float *)0x70003A24 = 4.5f;
        break;
    case 0xBF:
    case 0xC7:
        *(float *)0x70003A20 = 9.0f;
        *(float *)0x70003A24 = 9.0f;
        break;
    }

    *(float *)0x700038A0 = 0.0f;
    *(float *)0x700038A4 = 0.0f;
    *(float *)0x700038A8 = *(float *)0x70003A20;
    *(float *)0x700038AC = 0.0f;
    func_001026A0(D_700038B0, (float *)(e + 0xD0), D_700038A0);
    *(float *)(e + 0xB0) = *(float *)(e + 0x2F4) + *(float *)0x700038B0;
    *(float *)(e + 0xB8) = *(float *)(e + 0x2F8) + *(float *)0x700038B8;

    *(float *)0x700038A0 = 0.0f;
    *(float *)0x700038A4 = 0.0f;
    *(float *)0x700038A8 = *(float *)0x70003A24;
    *(float *)0x700038AC = 0.0f;
    func_001026A0(D_700038B0, (float *)(e + 0xD0), D_700038A0);
    *(float *)(e + 0x290) = *(float *)(e + 0x290) + *(float *)0x700038B0;
    *(float *)(e + 0x298) = *(float *)(e + 0x298) + *(float *)0x700038B8;
}
