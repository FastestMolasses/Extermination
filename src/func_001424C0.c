// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 96.45% NEARMISS).
// Fixes read from the original instructions (the old "prologue register
// permutation" wall was our spelling, not the compiler):
//  - Switch directly on the state byte arg0+6 and re-read it for the case-0
//    increment (structural twin func_00147B50); the cached `st` local was wrong.
//  - Case-0 timer is one expression `(int)((rand>>3 & 0x7F) + 0xB4) & 0xFFFF`
//    (keeps the original's 0xFFFF mask), random calls inline.
//  - Counter guards spelled `> 2` (idiom-28: the compare result goes to $at).
//  - The +0x7F sign tests are `(signed char)flag & 0x80` on an int-promoted
//    value: one 64-bit shift-extend pair, not two.
//  - The frame-gate scratchpad words 0x70003B68 / 0x70003B8A are relocated
//    externs (idiom-32), which keeps the 0x7000 upper half out of a branch slot.
//  - The +0x5C distance is loaded once into a local before the <50 / <=20 tests.
//  - anim_clip_init's trailing 0.0 is staged as `(float)zi` (zi = 0), which puts
//    the $f13 zero move ahead of the $f12 move as in the original (func_00137C80).
//
// AI behavior-state driver for an enemy/creature, dispatched on arg0+6:
//   state 0: seed a burst-cooldown (arg1+0x77=0, +0x75=2, +0x4C=0.4),
//     a random re-engage timer at +0x20 (0xB4..0x133 frames), a random facing
//     +0x44 via func_001B1470, then kick off anim_clip_init and advance state.
//   state 1: poll func_00146110() (a proximity/LOS-class check: 2=idle-out,
//     0=continue-normal, else=in-range-attack). In-range: turn toward target
//     via func_001B1470/func_0011DF78/func_00128350/func_001000E0 (checks the
//     turn is "close enough" -- within pi/8, passed as the double of the float
//     pi/8), bumping a burst counter at +0x77 that resets the facing every 3rd
//     success. Normal: func_001469B0 ticks the AI, then either a hurt/alert
//     branch (+0x64) picking states 5/6/2 by proximity (+0x5C; turns toward the
//     player when +0x5C is beyond 20 and below 50) and setting per-state
//     cooldowns, or a movement-pick branch building a scratch position pair at
//     D_700038A0/D_700038B0 and picking among idle-turn (+0x7F, negative =
//     abort to state 0), charge (func_001B13F0 @ 90 units) or two cooldown-
//     gated wander checks (@ 30 / 100 units) gated by a shared frame-parity
//     mask (0x70003B68 + 0x70003B8A) & 0x3F. Falls through to a shared
//     +0x20 cooldown-tick tail that also re-arms state 0 when the burst timer
//     lapses and +0x7F is non-positive.
//   state 2: once the facing (+0x44) equals the current heading (arg0+0xC4),
//     return to state 1.
// Tail (all states): smoothly turns the object's current heading (arg0+0xC4)
// toward the target heading (arg1+0x44) via func_001B12B0, using a turn-rate
// constant that is 2 deg/frame (0.0349 rad) normally or 4 deg/frame (0.0698
// rad) while +0x7F is non-zero. func_00146110 receives (self, d) in a0 / a1,
// which this function never rewrites before the call.
extern void anim_clip_init(void *self, int a, float b, float c);
extern int func_001000E0(int a, double b);
extern void func_00102948(void *dst, void *src);
extern float func_0011DF78(float a);
extern int func_00122BB8(void);
extern int func_00128350(float a);
extern int func_00146110(void *self, void *d);
extern void func_001469B0(void *arg0, void *arg1);
extern float func_001B1240(void *a, float b, float c);
extern float func_001B12B0(float a, float b, float c);
extern int func_001B13F0(void *a, void *b, float c);
extern float func_001B1470(float a);

extern float D_00810360;
extern float D_00810368;
extern float D_700038A0;
extern float D_700038B0;
extern int D_70003B68;
extern short D_70003B8A;

void func_001424C0(char *arg0, char *arg1) {
    float f20 = 0.03490658476948738f;
    int r;
    int cnt;

    switch (*(unsigned char *)(arg0 + 6)) {
    case 0:
        *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
        *(char *)(arg1 + 0x77) = 0;
        *(char *)(arg1 + 0x75) = 2;
        *(int *)(arg1 + 0x4C) = 0x3ECCCCCD;

        *(int *)(arg1 + 0x20) = (int) ((((func_00122BB8() >> 3) & 0x7F) + 0xB4) & 0xFFFF);
        *(float *)(arg1 + 0x44) = func_001B1470((6.2831855f * (float) ((func_00122BB8() >> 0xF) & 0xFF)) / 255.0f);

        {
            int zi = 0;
            float z = (float)zi;
            anim_clip_init(arg0, 0, 5.0f, z);
        }
        break;

    case 1:
        r = func_00146110(arg0, arg1);
        if (r == 2) {
            break;
        }
        if (r != 0) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;

            if (func_001000E0(
                    func_00128350(func_0011DF78(
                        func_001B1470(*(float *)(arg1 + 0x44) -
                                      func_001B1470(3.1415927f + *(float *)(arg0 + 0xC4))))),
                    0.392699092626571655273437500) != 0) {   /* (double)(float)(pi / 8) */
                cnt = *(signed char *)(arg1 + 0x77) + 1;
                *(signed char *)(arg1 + 0x77) = cnt;
                if ((signed char) cnt > 2) {
                    *(signed char *)(arg1 + 0x77) = 0;
                    r = func_00122BB8();
                    *(float *)(arg1 + 0x44) =
                        func_001B1470(6.2831855f * ((float) ((r >> 0xC) & 0xFF) / 255.0f));
                }
            }
            goto tick;
        }

        func_001469B0(arg0, arg1);
        if (*(short *)(arg1 + 0x64) != 0) {
            if (*(signed char *)(arg1 + 0x78) == 0 && *(short *)(arg1 + 0x60) == 0) {
                if (!(*(float *)(arg1 + 0x5C) <= 40.0f)) {
                    if (((func_00122BB8() >> 0x11) & 7) > 2) {
                        *(short *)(arg1 + 0x60) = (short) ((func_00122BB8() >> 0xB) & 0x7F);
                        goto mid;
                    }
                    goto notice;
                }
notice:
                *(signed char *)(arg0 + 5) = 5;
                *(unsigned char *)(arg0 + 6) = 0;
            } else {
                float dist;
mid:
                if (*(unsigned char *)(arg1 + 0x71) == 0 && *(float *)(arg1 + 0x5C) <= 19.0f) {
                    *(signed char *)(arg0 + 5) = 6;
                    *(unsigned char *)(arg0 + 6) = 0;
                } else {
                    dist = *(float *)(arg1 + 0x5C);
                    if (!(dist < 50.0f)) {
                        *(signed char *)(arg0 + 5) = 2;
                        *(unsigned char *)(arg0 + 6) = 0;
                        *(short *)(arg1 + 0x62) =
                            (short) (((func_00122BB8() >> 0xF) & 0xFF) + 0x12C);
                    } else {
                        /* beyond 20 (and below 50): turn toward the player */
                        if (!(dist <= 20.0f)) {
                            *(float *)(arg1 + 0x44) =
                                func_001B1240(arg0 + 0xB0, D_00810360, D_00810368);
                        }
                        goto tick;
                    }
                }
            }
        } else {
            signed char flag;

            func_00102948(&D_700038A0, arg0 + 0xB0);
            func_00102948(&D_700038B0, arg1 + 0x10);
            *(int *)0x700038B4 = 0;
            *(int *)0x700038A4 = 0;

            flag = *(signed char *)(arg1 + 0x7F);
            if (flag != 0) {
                f20 = 0.06981316953897476f;
                if ((signed char) flag & 0x80) {
                    *(signed char *)(arg0 + 5) = 0;
                    *(unsigned char *)(arg0 + 6) = 0;
                } else if (func_001B13F0(&D_700038A0, &D_700038B0, 90.0f) == 0) {
                    *(signed char *)(arg0 + 5) = 2;
                    *(unsigned char *)(arg0 + 6) = 0;
                }
            } else if (*(signed char *)(arg1 + 0x76) != 0) {
                if (!((D_70003B68 + D_70003B8A) & 0x3F)) {
                    if (func_001B13F0(&D_700038A0, &D_700038B0, 30.0f) != 0) {
                        *(signed char *)(arg1 + 0x76) = 0;
                    }
                    *(float *)(arg1 + 0x44) =
                        func_001B1240(arg0 + 0xB0, *(float *)(arg1 + 0x10), *(float *)(arg1 + 0x18));
                }
            } else if (!((D_70003B68 + D_70003B8A) & 0x3F) &&
                       func_001B13F0(&D_700038A0, &D_700038B0, 100.0f) == 0) {
                *(signed char *)(arg1 + 0x76) = 1;
            }

tick:
            if (*(int *)(arg1 + 0x20) != 0) {
                *(int *)(arg1 + 0x20) = *(int *)(arg1 + 0x20) - 1;
                goto out;
            } else {
                signed char flag2 = *(signed char *)(arg1 + 0x7F);
                if (flag2 == 0 || ((signed char) flag2 & 0x80)) {
                    *(signed char *)(arg0 + 5) = 0;
                    *(unsigned char *)(arg0 + 6) = 0;
                }
            }
        }
        break;

    case 2:
        if (*(float *)(arg1 + 0x44) == *(float *)(arg0 + 0xC4)) {
            *(unsigned char *)(arg0 + 6) = 1;
        }
        break;
    }

out:
    *(float *)(arg0 + 0xC4) =
        func_001B12B0(*(float *)(arg1 + 0x44), *(float *)(arg0 + 0xC4), f20);
}
