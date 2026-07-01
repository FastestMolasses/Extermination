// NEARMISS func_001424C0  (vram 0x001424C0, 0x510 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 96.18% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Prologue register-allocation permutation (switch-discriminant register: target reuses $a2, mwcc allocates $a0) -- same confirmed-uncontrollable class as func_001B8AB0; residual after that is <4% of instructions.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// NEARMISS 96.18% (mwcc 2.3.3; 991202 = 89.3%). AI behavior-state driver for
// an enemy/creature, dispatched on arg0+6:
//   state 0: seed a burst-cooldown (arg1+0x77=0, +0x75=2, +0x4C=0.4),
//     a random re-engage timer at +0x20 (0xB4..0x133 frames), a random facing
//     +0x44 via func_001B1470, then kick off anim_clip_init and advance state.
//   state 1: poll func_00146110() (a proximity/LOS-class check: 2=idle-out,
//     0=continue-normal, else=in-range-attack). In-range: turn toward target
//     via func_001B1470/func_0011DF78/func_00128350/func_001000E0 (checks the
//     turn is "close enough" -- within pi/8), bumping a burst counter at
//     +0x77 that resets the facing every 3rd success. Normal: func_001469B0
//     ticks the AI, then either a hurt/alert branch (+0x64) picking states
//     5/6/2 by proximity (+0x5C) and setting per-state cooldowns, or a
//     movement-pick branch building a scratch position pair at
//     D_700038A0/D_700038B0 and picking among idle-turn (+0x7F, negative =
//     abort to state 0), charge (func_001B13F0 @ 90 units) or two cooldown-
//     gated wander checks (@ 30 / 100 units) gated by a shared frame-parity
//     mask (0x70003B68 + 0x70003B8A) & 0x3F. Falls through to a shared
//     +0x20 cooldown-tick tail that also re-arms state 0 when the burst timer
//     lapses and +0x7F is non-positive.
//   state 2: once the facing (+0x44) reaches the target orientation (arg0+0xC4)
//     exactly, advance to state... (then falls through to the shared tail).
// Tail (all states): smoothly turns the object's current heading (arg0+0xC4)
// toward the target heading (arg1+0x44) via func_001B12B0, using a turn-rate
// constant that is 2 deg/frame (0.0349 rad) normally or 4 deg/frame (0.0698
// rad) while movement is being actively re-picked (+0x7F != 0).
//
// Fully recovered: signed-char sign-extend idiom for the +0x7F/+0x77 bitfield
// checks (target performs an explicit 64-bit shift-extend before the 0x80
// sign-bit test -- reproduced via the (long long)(x<<0x38)>>0x38 cast), the
// fall-through-preferred if/else ordering the target's branch-likely-style
// scheduler expects (the >=50.0 "pick state 2" arm must be the ELSE of
// `!(x < 50.0f)`, not vice versa -- this alone took the match from ~72% to
// ~96%), and func_001000E0's true (int,double) signature (the pi/8 threshold
// is a double constant, not a 64-bit hex bit-hack).
//
// WALL: prologue register-allocation permutation, same class as
// func_001B8AB0 -- target keeps the incoming arg0 in $a2 across the state-byte
// load (paddub s1,a0,zero comes AFTER lbu a2,6(a0), using a2 not a0 for the
// switch value), mwcc picks a fresh $a0. Confirmed not source-controllable.
extern void anim_clip_init(void *self, int a, float b, float c);
extern int func_001000E0(int a, double b);
extern void func_00102948(void *dst, void *src);
extern float func_0011DF78(float a);
extern int func_00122BB8(void);
extern int func_00128350(float a);
extern int func_00146110(void);
extern void func_001469B0(void *arg0, void *arg1);
extern float func_001B1240(void *a, float b, float c);
extern float func_001B12B0(float a, float b, float c);
extern int func_001B13F0(void *a, void *b, float c);
extern float func_001B1470(float a);

extern float D_00810360;
extern float D_00810368;
extern float D_700038A0;
extern float D_700038B0;

void func_001424C0(char *arg0, char *arg1) {
    float f20 = 0.03490658476948738f;
    unsigned char st;
    int r;

    st = *(unsigned char *)(arg0 + 6);
    switch (st) {
    case 0:
        *(unsigned char *)(arg0 + 6) = st + 1;
        *(char *)(arg1 + 0x77) = 0;
        *(char *)(arg1 + 0x75) = 2;
        *(int *)(arg1 + 0x4C) = 0x3ECCCCCD;

        r = func_00122BB8();
        *(int *)(arg1 + 0x20) = (((r >> 3) & 0x7F) + 0xB4) & 0xFFFF;

        r = func_00122BB8();
        *(float *)(arg1 + 0x44) =
            func_001B1470((6.2831855f * (float) ((r >> 0xF) & 0xFF)) / 255.0f);

        anim_clip_init(arg0, 0, 0x40A00000, 0.0f);
        break;

    case 1:
        r = func_00146110();
        if (r == 2) {
            break;
        }
        if (r != 0) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;

            if (func_001000E0(
                    func_00128350(func_0011DF78(
                        func_001B1470(*(float *)(arg1 + 0x44) -
                                      func_001B1470(3.1415927f + *(float *)(arg0 + 0xC4))))),
                    0.39269908169872414) != 0) {
                signed char cnt = *(signed char *)(arg1 + 0x77) + 1;
                *(signed char *)(arg1 + 0x77) = cnt;
                if (cnt >= 3) {
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
                    if (((func_00122BB8() >> 0x11) & 7) >= 3) {
                        *(short *)(arg1 + 0x60) = (short) ((func_00122BB8() >> 0xB) & 0x7F);
                        goto mid;
                    }
                    goto notice;
                }
notice:
                *(signed char *)(arg0 + 5) = 5;
                *(unsigned char *)(arg0 + 6) = 0;
            } else {
mid:
                if (*(unsigned char *)(arg1 + 0x71) == 0 && *(float *)(arg1 + 0x5C) <= 19.0f) {
                    *(signed char *)(arg0 + 5) = 6;
                    *(unsigned char *)(arg0 + 6) = 0;
                } else if (!(*(float *)(arg1 + 0x5C) < 50.0f)) {
                    *(signed char *)(arg0 + 5) = 2;
                    *(unsigned char *)(arg0 + 6) = 0;
                    *(short *)(arg1 + 0x62) =
                        (short) (((func_00122BB8() >> 0xF) & 0xFF) + 0x12C);
                } else {
                    if (*(float *)(arg1 + 0x5C) <= 20.0f) {
                        *(float *)(arg1 + 0x44) =
                            func_001B1240(arg0 + 0xB0, D_00810360, D_00810368);
                    }
                    goto tick;
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
                if (((long long) ((long long) flag << 0x38) >> 0x38) & 0x80) {
                    *(signed char *)(arg0 + 5) = 0;
                    *(unsigned char *)(arg0 + 6) = 0;
                } else if (func_001B13F0(&D_700038A0, &D_700038B0, 90.0f) == 0) {
                    *(signed char *)(arg0 + 5) = 2;
                    *(unsigned char *)(arg0 + 6) = 0;
                }
            } else if (*(signed char *)(arg1 + 0x76) != 0) {
                if (!((*(int *)0x70003B68 + *(short *)0x70003B8A) & 0x3F)) {
                    if (func_001B13F0(&D_700038A0, &D_700038B0, 30.0f) != 0) {
                        *(signed char *)(arg1 + 0x76) = 0;
                    }
                    *(float *)(arg1 + 0x44) =
                        func_001B1240(arg0 + 0xB0, *(float *)(arg1 + 0x10), *(float *)(arg1 + 0x18));
                }
            } else if (!((*(int *)0x70003B68 + *(short *)0x70003B8A) & 0x3F) &&
                       func_001B13F0(&D_700038A0, &D_700038B0, 100.0f) == 0) {
                *(signed char *)(arg1 + 0x76) = 1;
            }

tick:
            if (*(int *)(arg1 + 0x20) != 0) {
                *(int *)(arg1 + 0x20) = *(int *)(arg1 + 0x20) - 1;
                goto out;
            } else {
                signed char flag2 = *(signed char *)(arg1 + 0x7F);
                if (flag2 == 0 || (((long long) ((long long) flag2 << 0x38) >> 0x38) & 0x80)) {
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
