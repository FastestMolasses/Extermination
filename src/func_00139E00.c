// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a 97.03% NEARMISS).
// Fixes read from the original instructions:
//  - The player-position / climb-out block D_00810320..D_00810368, the
//    "climbing" flag byte D_008102B0 and D_00810324 are volatile: the
//    original keeps every access in source order (the D_008102B0 |= 2
//    store before the D_00810360 load, D_00810324 = 0 after the
//    D_00810320 store, D_00810368 read after both).
//  - func_001026A0 takes three pointers; D_700036A0 is passed uncast (the
//    (int) cast swapped the argument setup).
//  - The z argument of func_001B1270 reads arg0+0xB8 into a block-local
//    float first, which makes the original evaluate the second float
//    argument before the first. Callee position parameters are void *
//    (as in the byte-matched twin func_0013A3B0); the spring constants are
//    float stores (0.65f / 0.4f / 0.005f).
//
// The VU0 scratchpad writes at 0x700038A0..AC and 0x700038B0..BC are
// constant-address volatile stores; only the whole-vector addresses passed
// to the helper calls use the relocated D_700036A0/D_700038A0/D_700038B0
// symbols. func_001FBD50 is (self, a, b, f). The derived climb-out heights
// are stored directly in each if/else leaf, and the spring-easing clamps
// use the `if (!(cond)) {A} else if (other) {B}` guard.
//
// State 3 sets +0x44 = 0.4 and +0x48 = 0 when +0x44 is NOT below 0.4 (a
// cap), corrected 2026-10-02 against the original instructions.

// Wade/swim-recovery state-machine step (dispatched on state byte at
// arg0+6, states 0..3; state byte 0 falls through into state 1's body).
// State 0: primes a 0x12C-frame recovery timer (arg1+0x20), sets the
// "recovering" flag arg1+0x84 = 1, kicks the recovery-start sound
// func_001FBD50(arg0,0x817,0,300.0f), then falls through into state 1.
// State 1: waits out the timer (arg1+0x81 override or the +0x20 countdown
// hitting 0) or an earlier func_001B1560 depth probe; while waiting it
// eases the pitch/roll spring constants (+0x44/+0x48/+0x4C/+0x50) and
// drives +0x58/+0x5C toward a facing angle via func_001B1240/func_001B1270.
// Once the probe AND func_001A7B80 both clear, it snaps into the "climb
// out" sequence: resets the spring state, derives the climb-out height
// D_008104D4 (varies by crouch flag +0xD&0x80 and door-state D_0081070A),
// sets the global "climbing" bit D_008102B0|=2, computes the VU0-style
// climb-out translation/rotation blend at D_00810320..0x360/0x364/0x368
// and the VU0 scratchpad words at 0x700036A0/0x700038A0/0x700038B0, kicks
// clip 5, and hands the blend to func_001EFD90. Otherwise (still in the
// water) it eases +0xC4/+0x50 toward the current target angles and runs
// the two idle-water helpers func_0013BBB0/func_0013BA20. State 2: once
// the 0x1000 input bit fires, arms a short 0x50-frame lunge (clip 4,
// spring resets, +0x5C = pi/2) via anim_clip_init(arg0, 4, 0, 0). State 3:
// identical timer wait to state 1 but simpler -- caps +0x44 at 0.4 (sets
// 0.4 and +0x48 = 0 unless +0x44 is below 0.4) and eases +0x50 toward
// +0x5C. Shared tail (done != 0: states 0/1 finished the timer or the
// probe, or state 3's timer elapsed): resets the recovery state and the
// actor back to state 0, then seeds arg1+0x2E with a random offset --
// state 3's finish (done == 2) uses [0xB4..0x133], states 0/1 use
// [0..0x3F].
//
extern void anim_clip_init(char *self, int clip, float a, float b);
extern void func_001026A0(void *dst, void *n, void *src);
extern void func_00102760(void *a0, void *a1);
extern void func_001028B8(void *a, void *b, void *c);
extern void func_001029C0(int);
extern void func_00102C58(char *a, char *b, char *c);
extern int func_00122BB8(void);
extern void func_0013BA20(unsigned char *, unsigned char *);
extern void func_0013BBB0(char *, char *);
extern int func_001A7B80(char *p);
extern float func_001B1240(void *pos, float px, float pz);
extern float func_001B1270(void *pos, float px, float pz);
extern float func_001B12B0(float a, float b, float c);
extern int func_001B1560(unsigned char *p, void *v, float r);
extern void func_001EFD90(int mode, void *b, void *c);
extern void func_001FBD50(char *p, int a, int b, float f);
extern int func_0021BE40(float *p, char *a);
extern volatile unsigned char D_008102B0;
extern volatile float D_00810320;
extern volatile int D_00810324;
extern volatile float D_00810328;
extern volatile float D_00810360;
extern volatile float D_00810364;
extern volatile float D_00810368;
extern int D_008104D4;
extern unsigned char D_0081070A;
extern int D_700036A0[4];
extern int D_700038A0[4];
extern int D_700038B0[4];

void func_00139E00(char *arg0, char *arg1) {
    unsigned char st = *(unsigned char *)(arg0 + 6);
    int done = 0;
    float f44;
    unsigned short timer;

    switch (st) {
    case 0:
        *(unsigned char *)(arg0 + 6) = st + 1;
        *(signed char *)(arg1 + 0x84) = 1;
        *(unsigned short *)(arg1 + 0x20) = 0x12C;
        func_001FBD50(arg0, 0x817, 0, 300.0f);
        /* fallthrough */
    case 1:
        if (*(signed char *)(arg1 + 0x81) != 0 ||
            (timer = *(unsigned short *)(arg1 + 0x20) - 1,
             *(unsigned short *)(arg1 + 0x20) = timer,
             (timer & 0xFFFF) == 0)) {
            done = 1;
        } else {
            if (func_001B1560((unsigned char *)arg0, &D_00810360, 1.483529806f) == 0) {
                done = 1;
            }
            *(int *)(arg1 + 0x48) = 0;
            f44 = *(float *)(arg1 + 0x44);
            if (!(f44 <= 0.8f)) {
                *(float *)(arg1 + 0x44) = 0.8f;
            } else if (f44 < 1.0f) {
                *(float *)(arg1 + 0x48) = 0.005f;
            }
            if (*(float *)(arg1 + 0x50) < 0.0f) {
                *(float *)(arg1 + 0x4C) = 0.65f;
            } else {
                *(float *)(arg1 + 0x4C) = 0.4f;
            }
            {
                float zz = *(float *)(arg0 + 0xB8);
                *(float *)(arg1 + 0x5C) = -1.0f * func_001B1270(arg0 + 0xB0, 7.0f + D_00810364, 5.0f + zz);
            }
            *(float *)(arg1 + 0x58) = func_001B1240(arg0 + 0xB0, D_00810360, D_00810368);
            if (func_0021BE40((float *)&D_008102B0, arg0) == 0 && func_001A7B80(arg0) != 0) {
                *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
                *(int *)(arg0 + 0xC8) = 0;
                *(float *)(arg1 + 0x4C) = 0.4f;
                *(int *)(arg1 + 0x48) = 0;
                *(float *)(arg1 + 0x44) = 0.0f;
                *(float *)(arg1 + 0x50) = 0.0f;
                if (*(unsigned char *)(arg0 + 0xD) & 0x80) {
                    if (D_0081070A != 0) {
                        D_008104D4 = 0x41C80000;
                    } else {
                        D_008104D4 = 0x41B00000;
                    }
                } else {
                    if (D_0081070A != 0) {
                        D_008104D4 = 0x41A00000;
                    } else {
                        D_008104D4 = 0x41900000;
                    }
                }
                D_008102B0 |= 2;
                D_00810320 = D_00810360 - *(float *)(arg0 + 0xB0);
                D_00810324 = 0;
                D_00810328 = D_00810368 - *(float *)(arg0 + 0xB8);
                func_00102760(&D_00810320, &D_00810320);
                anim_clip_init(arg0, 5, 0.0f, 0.0f);
                *(volatile int *)0x700038A8 = 0;
                *(volatile int *)0x700038A4 = 0;
                *(volatile int *)0x700038A0 = 0;
                *(volatile int *)0x700038AC = 0x3F800000;
                func_001029C0((int)&D_700036A0);
                *(volatile float *)0x700038A4 = func_001B1240((unsigned char *)&D_00810360, *(float *)(arg0 + 0xB0), *(float *)(arg0 + 0xB8));
                func_00102C58((char *)&D_700036A0, (char *)&D_700036A0, (char *)&D_700038A0);
                *(volatile int *)0x700038B0 = 0;
                *(volatile float *)0x700038B4 = (*(float *)(arg0 + 0xB4) - D_00810364) - 1.5f;
                *(volatile int *)0x700038B8 = 0x40400000;
                *(volatile int *)0x700038BC = 0x3F800000;
                func_001026A0(&D_700038B0, &D_700036A0, &D_700038B0);
                func_001028B8(&D_700038B0, &D_00810360, &D_700038B0);
                func_001EFD90(0x80000006, &D_700038B0, &D_700038A0);
            } else {
                *(float *)(arg0 + 0xC4) = func_001B12B0(*(float *)(arg1 + 0x58), *(float *)(arg0 + 0xC4), 0.008726646f);
                *(float *)(arg1 + 0x50) = func_001B12B0(*(float *)(arg1 + 0x5C), *(float *)(arg1 + 0x50), 0.031415924f);
                func_0013BBB0(arg0, arg1);
                func_0013BA20((unsigned char *)arg0, (unsigned char *)arg1);
            }
        }
        break;
    case 2:
        if (*(int *)(arg1 + 0x70) & 0x1000) {
            *(unsigned char *)(arg0 + 6) = st + 1;
            *(unsigned short *)(arg1 + 0x20) = 0x50;
            *(float *)(arg1 + 0x44) = -0.4f;
            *(int *)(arg1 + 0x48) = 0x3C23D70A;
            *(float *)(arg1 + 0x5C) = 1.5707964f;
            anim_clip_init(arg0, 4, 0.0f, 0.0f);
        }
        break;
    case 3:
        if (*(signed char *)(arg1 + 0x81) != 0 ||
            (timer = *(unsigned short *)(arg1 + 0x20) - 1,
             *(unsigned short *)(arg1 + 0x20) = timer,
             (timer & 0xFFFF) == 0)) {
            done = 2;
        }
        if (!(*(float *)(arg1 + 0x44) < 0.4f)) {
            *(float *)(arg1 + 0x44) = 0.4f;
            *(int *)(arg1 + 0x48) = 0;
        }
        *(float *)(arg1 + 0x50) = func_001B12B0(*(float *)(arg1 + 0x5C), *(float *)(arg1 + 0x50), 0.031415924f);
        break;
    }
    if (done != 0) {
        *(signed char *)(arg0 + 5) = 1;
        *(unsigned char *)(arg0 + 6) = 0;
        *(float *)(arg1 + 0x5C) = 0.0f;
        *(float *)(arg1 + 0x4C) = 0.4f;
        *(int *)(arg1 + 0x48) = 0;
        *(float *)(arg1 + 0x44) = 0.4f;
        *(unsigned short *)(arg1 + 0x20) = 0;
        *(short *)(arg1 + 0x2C) = 0;
        *(short *)(arg1 + 0x30) = 0;
        *(signed char *)(arg1 + 0x84) = 0;
        if (done == 2) {
            *(short *)(arg1 + 0x2E) = ((func_00122BB8() >> 15) & 0x7F) + 0xB4;
        } else {
            *(short *)(arg1 + 0x2E) = (func_00122BB8() >> 15) & 0x3F;
        }
    }
}
