// NEARMISS func_00147B50  (vram 0x00147B50, 0x5D4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.25% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// 99.25%, byte count matches target exactly (0x5D4). Two tiny residuals, both compiler-internal register/scheduling choices, not logic: (1) a throwaway comparison result placed in $v0 instead of the target's $at for one slti/bnez; (2) one redundant lui $at,0x7000 filling an otherwise-nop delay slot...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Entity state-machine dispatch on mode byte arg0+6 (0/1/2), driving a melee
// wind-up/attack cycle for entity arg0 with per-instance scratch arg1:
//  case 0: init arg1's cycle timer (arg1+0x4C=0.3), pick a random duration
//   (arg1+0x28) and random initial facing offset (arg1+0x44) from
//   func_00122BB8() (LCG rand), and if arg0's clip flag (arg0+0x2C bit) isn't
//   already clip 2, kick off anim_clip_init(arg0, 2, 5.0f, 0.0f).
//  case 1: snapshot arg0's current yaw into arg1+0x58, call func_0014B7B0
//   (attack-availability probe). If it returns 2, done for this tick. If
//   nonzero (not-ready), advance mode and, once the angle-to-target delta
//   (via func_001B1470 wrap) exceeds a threshold (pi/8, func_001000E0), bump
//   a retry counter arg1+0x84 and re-roll the facing offset after 3 misses.
//   If it returns 0 (ready), reset the counter, call func_0014BB10, then:
//     - if arg1+0x38 (a "locked-on" flag) is nonzero: run the strafe/attack
//       probe chain (func_001B13F0/func_001B1560/func_0014C130) that picks
//       the follow-up state (arg0+5 = 7/8/2) via the scratch (0x700038xx)
//       transform block, OR (when out of range) recompute arg1+0x44 via
//       func_001B1240.
//     - else: tick down arg1+0x28 (a cooldown), and run a periodic
//       (mod-0x40 frame gate) approach-transform refresh into the
//       0x700038A0/0x700038B0 scratch pair, toggling arg1+0x82 based on
//       func_001B13F0's angle check.
//  case 2: target yaw-lerp rate is pi/40; once arg0+0xC4 catches up to
//   arg1+0x44, advance mode back to 1.
//  Always ends by yaw-lerping arg0+0xC4 toward arg1+0x44 at the per-case rate
//  var_f20 (default pi/72, pi/36 in the "not locked on" sub-branch of case 1,
//  pi/40 in case 2) via func_001B12B0.
//
// Logic and structure fully recovered (byte-identical body, size matches
// target exactly: 0x5D4). Two residual artifacts, both instruction-scheduling/
// register-naming only (not logic): (1) one `slti $v0,$v0,3` / `bnez $v0,...`
// where the target uses `$at` for the same throwaway compare result (idiom-16
// int-cache pattern applied; register choice itself is compiler-internal and
// not C-addressable); (2) one redundant `lui $at,0x7000` duplicated into an
// otherwise-unused delay slot right after the arg1+0x82 dispatch (target
// leaves that slot a `nop`). Both are the documented CW/mwcc delay-slot-fill
// and temp-register-choice wall family (PROGRESS.md idiom 13), not
// C-fixable without permuter-class register forcing.
extern void anim_clip_init(char *self, int clip, float speed, float trailing);
extern int func_001000E0(int, double);
extern int func_001028B8(void *a, void *b, void *c);
extern float func_0011DF78(float);
extern int func_00122BB8(void);
extern int func_00128350(float);
extern int func_0014B7B0(void *self, void *out);
extern void func_0014BB10(void *arg0, void *arg1);
extern int func_0014BC30(void *arg0, void *arg1);
extern int func_0014C130(void *arg0);
extern float func_001B1240(void *a, float b, float c);
extern float func_001B12B0(float goal, float cur, float rate);
extern int func_001B13F0(void *a, void *b, float deg);
extern float func_001B1470(float);
extern int func_001B1560(void *a, void *b, float v);
extern void func_001B2B10(void *self, void *a, void *b);
extern int func_001B3250(void *self, void *v, float r);

extern float D_00810360;
extern float D_00810364;
extern float D_00810368;
extern float D_008104D8;
extern int D_700038A0;
extern int D_700038B0;

void func_00147B50(char *arg0, char *arg1) {
    float var_f20;
    int r;
    int cnt;

    var_f20 = 0.04363323375582695f;
    switch (*(unsigned char *)(arg0 + 6)) {
    case 0:
        *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
        *(char *)(arg1 + 0x84) = 0;
        *(int *)(arg1 + 0x4C) = 0x3E99999A;
        *(int *)(arg1 + 0x28) = (int) ((((func_00122BB8() >> 3) & 0x7F) + 0xB4) & 0xFFFF);
        *(float *)(arg1 + 0x44) = func_001B1470((6.2831855f * (float) ((func_00122BB8() >> 0x10) & 0xFF)) / 255.0f);
        if ((*(short *)(arg0 + 0x2C) & 0xFFFF7FFF) != 2) {
            anim_clip_init(arg0, 2, 5.0f, 0.0f);
            break;
        }
        break;
    case 1:
        *(float *)(arg1 + 0x58) = *(float *)(arg0 + 0xC4);
        r = func_0014B7B0(arg0, arg1);
        if (r == 2) {
            break;
        }
        if (r != 0) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
            if (func_001000E0(func_00128350(func_0011DF78(func_001B1470(*(float *)(arg1 + 0x44) - func_001B1470(3.1415927f + *(float *)(arg1 + 0x58))))), 0.39269909262657166) != 0) {
                cnt = *(signed char *)(arg1 + 0x84) + 1;
                *(signed char *)(arg1 + 0x84) = cnt;
                if ((signed char) cnt >= 3) {
                    *(signed char *)(arg1 + 0x84) = 0;
                    *(float *)(arg1 + 0x44) = func_001B1470(6.2831855f * ((float) ((func_00122BB8() >> 0xC) & 0xFF) / 255.0f));
                }
            }
        } else {
            *(unsigned char *)(arg1 + 0x84) = 0;
            func_0014BB10(arg0, arg1);
            if (*(int *)(arg1 + 0x38) == 0) {
                if (*(unsigned char *)(arg0 + 0xA) & 1) {
                    var_f20 = 0.0872664675116539f;
                    if (func_001B13F0(&D_00810360, arg0 + 0xB0, 20.0f) != 0) {
                        if (func_001B1560(arg0, &D_00810360, 1.5533430576324463f) != 0) {
                            if (func_0014C130(arg0) == 0) {
                                goto set8;
                            }
                            if (D_008104D8 < 100.0f) {
                                *(char *)(arg0 + 5) = 7;
                                *(unsigned char *)(arg0 + 6) = 0;
                            } else {
                            set8:
                                *(char *)(arg0 + 5) = 8;
                                *(unsigned char *)(arg0 + 6) = 0;
                            }
                            break;
                        }
                    } else {
                        *(float *)(arg1 + 0x44) = func_001B1240(arg0 + 0xB0, D_00810360, D_00810368);
                    }
                    if ((func_0014BC30(arg0, arg1) == 0) &&
                        (func_001B1560(arg0, &D_00810360, 0.0872664675116539f) != 0) &&
                        (func_001B13F0(&D_00810360, arg0 + 0xB0, 40.0f) == 0)) {
                        *(int *)0x700038A0 = 0;
                        *(int *)0x700038A4 = 0;
                        *(float *)0x700038A8 = 50.0f;
                        *(float *)0x700038AC = 1.0f;
                        func_001B2B10(arg0, &D_700038A0, &D_700038A0);
                        func_001028B8(&D_700038A0, arg0 + 0xB0, &D_700038A0);
                        if (func_001B3250(arg0, &D_700038A0, 15.0f) == 0) {
                            *(char *)(arg0 + 5) = 2;
                            *(unsigned char *)(arg0 + 6) = 0;
                        }
                    }
                    break;
                }
                if (*(int *)(arg1 + 0x28) != 0) {
                    *(int *)(arg1 + 0x28) = *(int *)(arg1 + 0x28) - 1;
                } else {
                    *(char *)(arg0 + 5) = 0;
                    *(unsigned char *)(arg0 + 6) = 0;
                }
                if (*(signed char *)(arg1 + 0x82) != 0) {
                    if (!((*(int *)0x70003B68 + *(short *)0x70003B8A) & 0x3F)) {
                        *(float *)0x700038A0 = *(float *)(arg0 + 0xB0);
                        *(float *)0x700038A4 = *(float *)(arg0 + 0xB4);
                        *(float *)0x700038A8 = *(float *)(arg0 + 0xB8);
                        *(float *)0x700038AC = *(float *)(arg0 + 0xBC);
                        *(float *)0x700038B0 = *(float *)(arg1 + 0);
                        *(float *)0x700038B4 = *(float *)(arg1 + 4);
                        *(float *)0x700038B8 = *(float *)(arg1 + 8);
                        *(float *)0x700038BC = *(float *)(arg1 + 0xC);
                        *(float *)0x700038B4 = 0.0f;
                        *(float *)0x700038A4 = 0.0f;
                        if (func_001B13F0(&D_700038A0, &D_700038B0, 25.0f) != 0) {
                            *(signed char *)(arg1 + 0x82) = 0;
                        }
                        *(float *)(arg1 + 0x44) = func_001B1240(arg0 + 0xB0, *(float *)(arg1 + 0), *(float *)(arg1 + 8));
                    }
                } else if (!((*(int *)0x70003B68 + *(short *)0x70003B8A) & 0x3F)) {
                    *(float *)0x700038A0 = *(float *)(arg0 + 0xB0);
                    *(float *)0x700038A4 = *(float *)(arg0 + 0xB4);
                    *(float *)0x700038A8 = *(float *)(arg0 + 0xB8);
                    *(float *)0x700038AC = *(float *)(arg0 + 0xBC);
                    *(float *)0x700038B0 = *(float *)(arg1 + 0);
                    *(float *)0x700038B4 = *(float *)(arg1 + 4);
                    *(float *)0x700038B8 = *(float *)(arg1 + 8);
                    *(float *)0x700038BC = *(float *)(arg1 + 0xC);
                    *(float *)0x700038B4 = 0.0f;
                    *(float *)0x700038A4 = 0.0f;
                    if (func_001B13F0(&D_700038A0, &D_700038B0, 50.0f) == 0) {
                        *(signed char *)(arg1 + 0x82) = 1;
                    }
                }
            }
        }
        break;
    case 2:
        var_f20 = 0.07853981852531433f;
        if (*(float *)(arg0 + 0xC4) == *(float *)(arg1 + 0x44)) {
            *(unsigned char *)(arg0 + 6) = 1;
        }
        break;
    }
    *(float *)(arg0 + 0xC4) = func_001B12B0(*(float *)(arg1 + 0x44), *(float *)(arg0 + 0xC4), var_f20);
}
