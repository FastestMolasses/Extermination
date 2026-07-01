// NEARMISS func_00139E00  (vram 0x00139E00, 0x5A4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 96.86% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-coloring/scheduling permutation: func_001B1270's two float args are evaluated in the opposite order from the target (a genuine FP-arg-order swap that the assign-in-arg idiom did not crack), a small set of lui/addiu hoist-order swaps in the D_00810320/D_00810324/D_00810328 float block and...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

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
// spring resets) via anim_clip_init(arg0, 4, PI/2, 0). State 3: identical
// timer wait to state 1 but simpler -- just clamps +0x44 up to 0.4 and
// eases +0x50 toward +0x5C. Shared tail (var_s0 != 0, i.e. states 0/1
// finished the probe or state 2/3's timer elapsed): resets the recovery
// state and the actor back to state 0, then seeds arg1+0x2E with a
// random splash-frame offset -- states {0,1}-finished use a wider
// [0xB4..0x133) range, state 3 uses [0..0x3F).
//
// NEARMISS: 96.86% (mwcc 2.3.3), 89.02% (pinned 991202). Keys that
// cracked this from an initial ~85%: (1) the VU0 scratchpad writes at
// 0x700038A0/A4/A8/AC and 0x700038B0/B4/B8/BC are constant-address
// volatile stores (matching the anim_decode_translation family), NOT
// array-index writes through the D_700038A0 symbol -- only the
// *whole-struct address* passed to helper calls uses the relocated
// symbol form; (2) func_001FBD50's real signature is (self,a,b,f) with
// `self` an IMPLICIT/unset arg here (still holds `arg0` from function
// entry, never reloaded); (3) writing derived-constant assignments as
// direct `D_008104D4 = <imm>;` in each if/else leaf (not through an
// intermediate local) avoids mwcc emitting `movz`, matching the target's
// plain branch-per-leaf store shape; (4) `if (!(cond)) {A} else if
// (other) {B}` (De Morgan'd guard) reproduces the target's bc1tl/bc1f
// sense for the two spring-easing clamps. Residual: one FP call
// (func_001B1270) evaluates its two float args in the opposite order
// from ours (target computes the second arg first), a handful of
// lui/addiu scheduling-order swaps in the D_00810320 float block and the
// func_001026A0 D_700036A0/D_700038B0 argument setup, and the very last
// bc1t/bc1f sense on the state-3 easing call -- all register/scheduling
// only, no logic difference.
extern void anim_clip_init(char *self, int clip, float a, float b);
extern void func_001026A0(void *dst, int n, void *src);
extern void func_00102760(void *a0, void *a1);
extern void func_001028B8(void *a, void *b, void *c);
extern void func_001029C0(int);
extern void func_00102C58(char *a, char *b, char *c);
extern int func_00122BB8(void);
extern void func_0013BA20(unsigned char *, unsigned char *);
extern void func_0013BBB0(char *, char *);
extern int func_001A7B80(char *p);
extern float func_001B1240(unsigned char *origin, float x, float z);
extern float func_001B1270(unsigned char *origin, float x, float z);
extern float func_001B12B0(float a, float b, float c);
extern int func_001B1560(unsigned char *p, void *v, float r);
extern void func_001EFD90(int mode, void *b, void *c);
extern void func_001FBD50(char *p, int a, int b, float f);
extern int func_0021BE40(float *p, char *a);
extern unsigned char D_008102B0;
extern float D_00810320;
extern int D_00810324;
extern float D_00810328;
extern float D_00810360;
extern float D_00810364;
extern float D_00810368;
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
                *(int *)(arg1 + 0x48) = 0x3BA3D70A;
            }
            if (*(float *)(arg1 + 0x50) < 0.0f) {
                *(int *)(arg1 + 0x4C) = 0x3F266666;
            } else {
                *(int *)(arg1 + 0x4C) = 0x3ECCCCCD;
            }
            *(float *)(arg1 + 0x5C) = -1.0f * func_001B1270((unsigned char *)(arg0 + 0xB0), 7.0f + D_00810364, 5.0f + *(float *)(arg0 + 0xB8));
            *(float *)(arg1 + 0x58) = func_001B1240((unsigned char *)(arg0 + 0xB0), D_00810360, D_00810368);
            if (func_0021BE40((float *)&D_008102B0, arg0) == 0 && func_001A7B80(arg0) != 0) {
                *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
                *(int *)(arg0 + 0xC8) = 0;
                *(int *)(arg1 + 0x4C) = 0x3ECCCCCD;
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
                func_001026A0(&D_700038B0, (int)&D_700036A0, &D_700038B0);
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
        if (*(float *)(arg1 + 0x44) < 0.4f) {
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
        *(int *)(arg1 + 0x4C) = 0x3ECCCCCD;
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
