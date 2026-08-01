// NEARMISS func_0013F770  (vram 0x0013F770, 0xCCC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.49% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Body, control flow, all constants and both epilogue shapes are byte-identical; a single 7-instruction window remains in case 2 around `*(volatile float*)0x700038A4 += 10.0f`. CodeWarrior materialises the two following scratchpad constants (9.0f then 1.0f) one at a time into $v0, each immediately ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// SEMANTICS: 9-state grapple/throw AI driver for actor `self`, dispatched on the
// state byte at self+6 (jtbl_0026D240) and driving the owner context `ctx`.
//   0  entry: reset the ctx timers/flags, kick clip 0x14 and cue 0x808.
//   1  steer the actor's facing (self+0xC4) toward the target bearing returned by
//      func_001B1240 at 4 deg/frame; on the owner's 0x1000 input bit advance and
//      kick clip 0x15.
//   2  on the same input bit: latch pose 0x80, lunge timers (1.5 / 0.85), clip 0x16
//      + cue 0x80B; build the two scratchpad probe vectors (0,0,2.5) and (0,0,9)
//      offset by ctx+0x60 and +10 units, and if func_001B3390 reports a hit within
//      45 degrees, cancel the lunge.
//   3  grab window: if the player is grabbable (func_0021BED0 == 0 and
//      func_001A7B80) latch the grab -- set player flag bit 1, snap the player to
//      the actor's hand matrix (self+0x14C + 0x90) rotated by (0.919, 0.219, 0),
//      pick a throw distance 25 or 30 (30 when D_0081070A), and either bail to
//      state 8 (clip 0x1A) when the wall distance D_008104D0 is shorter, or
//      commit: face 180 degrees away, clip 0x17, place the player at 6.35 units.
//      Otherwise fall to state 8 when the lunge timer expires or the hit bit is set.
//   4  hold: keep the player parented at 6.35 units; on the input bit advance and
//      kick clip 0x18.
//   5  windup: same parenting; D_008106BD == 1 aborts to state 6 (clip 0x1C).
//      After 180 frames throw: cue 0x80A, re-snap the player, distance 12/15,
//      clip 0x19 (or abort to state 6 when the wall is closer).  A second 130-frame
//      counter re-fires cue 0x808 with a randomized phase.
//   6  recover: once the lunge timer runs out, spawn the hit probe (clip pose flag
//      bit 1) and on the hit bit advance to state 7 with clip 0x1B; while the timer
//      is still running and the sweep value is negative, run the two-vector
//      45-degree wall probe and cancel the sweep on a hit.
//   7  on the input bit, hand the actor back to behaviour 4 and reset to state 0.
//   8  bail-out mirror of state 6 that returns to state 7 with clip 8 and cue 0x7F9.
//
// NEARMISS 99.486% (mwcc 2.3.3; 86.90% on the pinned 2.3.1.01 build).  Body,
// control flow and every constant are byte-identical except for one 7-instruction
// window in state 2 around `*(0x700038A4) += 10.0f`: CodeWarrior materialises the
// two following scratchpad constants (9.0f, 1.0f) one at a time into $v0 and sinks
// `paddub a0,s1,zero` ahead of them, where mwcc co-hoists both `lui`s into $v1/$v0
// and sinks the paddub to the jal.  Also mwcc canonicalises the float literal to
// the `add.s` fs slot.  Fourteen source spellings of the add/reload were measured
// (see the run log); `t = load; t = t + 10.0f;` between the 38B4 and 38B8 stores is
// the best, and the two remaining shapes trade one residual for the other.
extern void anim_clip_init(char *self, int clip, float a, float b);
extern void copy_qw4(void *dst, void *src);
extern void func_001026A0(void *a, void *b, void *c);
extern void func_001028B8(void *a, void *b, void *c);
extern void func_00102948(void *dst, void *src);
extern void func_001029C0(void *m);
extern void func_00102BB0(void *a, void *b, float f);
extern int func_00122BB8(void);
extern int func_001A7B80(char *p);
extern float func_001B1240(void *p, float x, float z);
extern float func_001B12B0(float goal, float cur, float rate);
extern float func_001B1470(float a);
extern void func_001B2B10(void *self, void *a, void *b);
extern int func_001B3390(void *self, void *a, void *b, float f);
extern void func_001E8B90(void *p, float f);
extern void func_001EFD90(int a, void *b, void *c);
extern void func_001FBD50(char *self, int a, int b, float f);
extern int func_0021BED0(unsigned char *player);
extern void func_0021BF90(unsigned char *player, void *self);

extern unsigned char D_008102B0;
extern float D_00810350[4];
extern float D_00810360;
extern float D_00810368;
extern float D_00810370[4];
extern float D_00810374;
extern float D_008104D0;
extern float D_008104D4;
extern unsigned char D_008106BD;
extern unsigned char D_0081070A;
extern float D_700036A0[4];
extern float D_700038A0[4];
extern float D_700038B0[4];

void func_0013F770(char *self, char *ctx) {
    unsigned char st;
    char flag;
    float t;
    unsigned int c;
    int zi2;
    float z2;
    int zi3;
    float z3;
    int zi4;
    float z4;
    int zi6;
    float z6;

    st = *(unsigned char *)(self + 6);
    switch (st) {
    case 0:
        *(unsigned char *)(self + 6) = *(volatile unsigned char *)(self + 6) + 1;
        *(int *)(ctx + 0x20) = 0;
        *(char *)(ctx + 0x8D) = 1;
        *(char *)(ctx + 0x81) = 0;
        *(float *)(ctx + 0x40) = 1.0f;
        *(float *)(ctx + 0x48) = 0.0f;
        *(char *)(ctx + 0x86) = *(char *)(ctx + 0x86) & 1;
        *(int *)(ctx + 0x50) = 0;
        *(float *)(ctx + 0x4C) = 0.0f;
        anim_clip_init(self, 0x14, 5.0f, 0.0f);
        func_001FBD50(self, 0x808, 0, 300.0f);
        goto done;
    case 1:
        *(float *)(ctx + 0x44) = func_001B1240(self + 0xB0, D_00810360, D_00810368);
        *(float *)(self + 0xC4) = func_001B12B0(*(float *)(ctx + 0x44),
                                                *(float *)(self + 0xC4), 0.06981317f);
        if (*(int *)(ctx + 0x30) & 0x1000) {
            *(unsigned char *)(self + 6) = *(unsigned char *)(self + 6) + 1;
            *(char *)(ctx + 0x81) = 1;
            anim_clip_init(self, 0x15, 0.0f, 0.0f);
        }
        goto done;
    case 2:
        if (*(int *)(ctx + 0x30) & 0x1000) {
            *(unsigned char *)(self + 6) = st + 1;
            *(char *)(ctx + 0x81) = -0x80;
            *(float *)(ctx + 0x48) = 1.5f;
            *(float *)(ctx + 0x4C) = 0.85f;
            zi2 = 0;
            z2 = (float)zi2;
            anim_clip_init(self, 0x16, 0.0f, z2);
            func_001FBD50(self, 0x80B, 0, 300.0f);
            if (*(char *)(ctx + 0x86) == 1) {
                *(volatile float *)0x700038A0 = 0.0f;
                *(volatile float *)0x700038A4 = 0.0f;
                *(volatile float *)0x700038A8 = 2.5f;
                *(volatile float *)0x700038AC = 1.0f;
                func_001B2B10(self, D_700038A0, D_700038A0);
                func_001028B8(D_700038A0, self + 0xB0, D_700038A0);
                *(volatile float *)0x700038A4 = *(float *)(ctx + 0x60);
                func_001EFD90(0x80000016, D_700038A0, self + 0xC0);
                func_001E8B90(D_700038A0, 5.0f);
            }
            func_00102948(D_700038A0, self + 0xB0);
            *(volatile float *)0x700038B0 = 0.0f;
            *(volatile float *)0x700038B4 = 0.0f;
            t = *(volatile float *)0x700038A4;
            t = t + 10.0f;
            *(volatile float *)0x700038B8 = 9.0f;
            *(volatile float *)0x700038BC = 1.0f;
            *(volatile float *)0x700038A4 = t;
            func_001B2B10(self, D_700038B0, D_700038B0);
            func_001028B8(D_700038B0, D_700038B0, D_700038A0);
            if (func_001B3390(self, D_700038A0, D_700038B0, 0.7853982f)) {
                *(float *)(ctx + 0x4C) = 0.0f;
            }
        }
        goto done;
    case 3:
        *(char *)(ctx + 0x85) = 1;
        if (func_0021BED0(&D_008102B0) == 0 && func_001A7B80(self) != 0) {
            *(unsigned char *)(self + 6) = *(unsigned char *)(self + 6) + 1;
            D_008102B0 |= 2;
            copy_qw4(D_700036A0, *(char **)(self + 0x14C) + 0x90);
            *(volatile float *)0x700038A0 = 0.919f;
            *(volatile float *)0x700038A4 = 0.219f;
            *(volatile float *)0x700038A8 = 0.0f;
            *(volatile float *)0x700038AC = 1.0f;
            func_001026A0(D_700038A0, D_700036A0, D_700038A0);
            func_001EFD90(0x80000006, D_700038A0, D_00810370);
            if (D_0081070A != 0) {
                *(volatile float *)0x70003A20 = 30.0f;
            } else {
                *(volatile float *)0x70003A20 = 25.0f;
            }
            if (D_008104D0 <= (t = *(volatile float *)0x70003A20)) {
                *(unsigned char *)(self + 6) = 8;
                D_008104D4 = D_008104D0;
                zi3 = 0;
                z3 = (float)zi3;
                anim_clip_init(self, 0x1A, 0.0f, z3);
                goto done;
            }
            D_008104D4 = t;
            func_0021BF90(&D_008102B0, self);
            *(char *)(ctx + 0x89) = 1;
            D_00810374 = func_001B1470(3.1415927f + *(float *)(self + 0xC4));
            anim_clip_init(self, 0x17, 0.0f, 0.0f);
            func_001029C0(D_700036A0);
            func_00102BB0(D_700036A0, D_700036A0, D_00810374);
            *(volatile float *)0x700038A0 = 0.0f;
            *(volatile float *)0x700038A4 = 0.0f;
            *(volatile float *)0x700038A8 = 6.35f;
            *(volatile float *)0x700038AC = 1.0f;
            func_001026A0(D_700038A0, D_700036A0, D_700038A0);
            func_001028B8(D_700038A0, D_00810350, D_700038A0);
            func_00102948(self + 0xB0, D_700038A0);
            *(char *)(ctx + 0x81) = -0x7E;
            goto done;
        }
        if (*(float *)(ctx + 0x48) < 0.0f || (*(char *)(ctx + 0x80) & 2)) {
            *(unsigned char *)(self + 6) = 8;
            anim_clip_init(self, 0x1A, 0.0f, 0.0f);
        }
        goto done;
    case 4:
        *(char *)(ctx + 0x85) = 1;
        func_001029C0(D_700036A0);
        func_00102BB0(D_700036A0, D_700036A0, D_00810374);
        *(volatile float *)0x700038A0 = 0.0f;
        *(volatile float *)0x700038A4 = 0.0f;
        *(volatile float *)0x700038A8 = 6.35f;
        *(volatile float *)0x700038AC = 1.0f;
        func_001026A0(D_700038A0, D_700036A0, D_700038A0);
        func_001028B8(D_700038A0, D_00810350, D_700038A0);
        func_00102948(self + 0xB0, D_700038A0);
        *(float *)(ctx + 0x48) = 0.0f;
        *(int *)(ctx + 0x50) = 0;
        *(float *)(ctx + 0x4C) = 0.0f;
        if (*(int *)(ctx + 0x30) & 0x1000) {
            *(unsigned char *)(self + 6) = *(unsigned char *)(self + 6) + 1;
            *(int *)(ctx + 0x20) = 0;
            *(int *)(ctx + 0x24) = 0;
            zi4 = 0;
            z4 = (float)zi4;
            anim_clip_init(self, 0x18, 0.0f, z4);
        }
        goto done;
    case 5:
        *(char *)(ctx + 0x85) = 1;
        func_001029C0(D_700036A0);
        func_00102BB0(D_700036A0, D_700036A0, D_00810374);
        *(volatile float *)0x700038A0 = 0.0f;
        *(volatile float *)0x700038A4 = 0.0f;
        *(volatile float *)0x700038A8 = 6.35f;
        *(volatile float *)0x700038AC = 1.0f;
        func_001026A0(D_700038A0, D_700036A0, D_700038A0);
        func_001028B8(D_700038A0, D_00810350, D_700038A0);
        func_00102948(self + 0xB0, D_700038A0);
        *(float *)(ctx + 0x48) = 0.0f;
        *(int *)(ctx + 0x50) = 0;
        *(float *)(ctx + 0x4C) = 0.0f;
        if (D_008106BD == 1) {
            *(unsigned char *)(self + 6) = 6;
            *(char *)(ctx + 0x89) = 0;
            D_008106BD = 0;
            *(char *)(ctx + 0x81) = -0x80;
            *(float *)(ctx + 0x48) = 1.5f;
            *(float *)(ctx + 0x4C) = -0.45f;
            anim_clip_init(self, 0x1C, 0.0f, 0.0f);
            func_001FBD50(self, 0x80D, 0, 300.0f);
            goto done;
        }
        c = *(unsigned int *)(ctx + 0x20) + 1;
        *(unsigned int *)(ctx + 0x20) = c;
        if (c >= 0xB4) {
            *(int *)(ctx + 0x20) = 0;
            *(int *)(ctx + 0x24) = 0;
            D_008102B0 |= 2;
            func_001FBD50(self, 0x80A, 0, 300.0f);
            copy_qw4(D_700036A0, *(char **)(self + 0x14C) + 0x90);
            *(volatile float *)0x700038A0 = 0.919f;
            *(volatile float *)0x700038A4 = 0.219f;
            *(volatile float *)0x700038A8 = 0.0f;
            *(volatile float *)0x700038AC = 1.0f;
            func_001026A0(D_700038A0, D_700036A0, D_700038A0);
            func_001EFD90(0x80000006, D_700038A0, D_00810370);
            if (D_0081070A != 0) {
                *(volatile float *)0x70003A20 = 15.0f;
            } else {
                *(volatile float *)0x70003A20 = 12.0f;
            }
            if (D_008104D0 <= (t = *(volatile float *)0x70003A20)) {
                *(unsigned char *)(self + 6) = 6;
                *(char *)(ctx + 0x89) = 0;
                D_008106BD = 0;
                *(char *)(ctx + 0x81) = -0x80;
                *(float *)(ctx + 0x48) = 1.3f;
                *(float *)(ctx + 0x4C) = -0.2f;
                D_008104D4 = D_008104D0;
                anim_clip_init(self, 0x1C, 0.0f, 0.0f);
                goto done;
            }
            D_008104D4 = t;
            anim_clip_init(self, 0x19, 0.0f, 0.0f);
        }
        c = *(unsigned int *)(ctx + 0x24) + 1;
        *(unsigned int *)(ctx + 0x24) = c;
        if (c >= 0x82) {
            *(int *)(ctx + 0x24) = (func_00122BB8() >> 19) & 0x3F;
            func_001FBD50(self, 0x808, 0, 300.0f);
        }
        goto done;
    case 6:
        *(char *)(ctx + 0x85) = 1;
        if (*(float *)(ctx + 0x48) <= 0.0f) {
            flag = *(char *)(ctx + 0x86);
            if (flag == 1) {
                *(char *)(ctx + 0x86) = flag | 2;
                *(volatile float *)0x700038A0 = 0.0f;
                *(volatile float *)0x700038A4 = 0.0f;
                *(volatile float *)0x700038A8 = 5.0f;
                *(volatile float *)0x700038AC = 1.0f;
                func_001B2B10(self, D_700038A0, D_700038A0);
                func_001028B8(D_700038A0, self + 0xB0, D_700038A0);
                *(volatile float *)0x700038A4 = *(float *)(ctx + 0x60);
                func_001EFD90(0x80000016, D_700038A0, self + 0xC0);
                func_001E8B90(D_700038A0, 5.0f);
            }
            if (*(char *)(ctx + 0x80) & 2) {
                *(unsigned char *)(self + 6) = *(unsigned char *)(self + 6) + 1;
                *(char *)(ctx + 0x81) = 1;
                *(float *)(ctx + 0x48) = 0.0f;
                *(int *)(ctx + 0x50) = 0;
                *(float *)(ctx + 0x4C) = 0.0f;
                zi6 = 0;
                z6 = (float)zi6;
                anim_clip_init(self, 0x1B, 0.0f, z6);
            }
        } else if (*(float *)(ctx + 0x4C) < 0.0f) {
            *(volatile float *)0x700038A0 = 0.0f;
            *(volatile float *)0x700038B0 = 0.0f;
            *(volatile float *)0x700038A4 = 3.0f;
            *(volatile float *)0x700038B4 = 3.0f;
            *(volatile float *)0x700038A8 = 4.0f;
            *(volatile float *)0x700038AC = 1.0f;
            *(volatile float *)0x700038B8 = -4.0f;
            *(volatile float *)0x700038BC = 1.0f;
            func_001B2B10(self, D_700038A0, D_700038A0);
            func_001B2B10(self, D_700038B0, D_700038B0);
            func_001028B8(D_700038A0, self + 0xB0, D_700038A0);
            func_001028B8(D_700038B0, self + 0xB0, D_700038B0);
            if (func_001B3390(self, D_700038A0, D_700038B0, 0.7853982f)) {
                *(float *)(ctx + 0x4C) = 0.0f;
            }
        }
        goto done;
    case 7:
        if (*(int *)(ctx + 0x30) & 0x1000) {
            *(char *)(self + 5) = 4;
            *(unsigned char *)(self + 6) = 0;
            *(char *)(ctx + 0x81) = 0;
            *(char *)(ctx + 0x85) = 0;
            *(char *)(ctx + 0x8D) = 0;
        }
        goto done;
    case 8:
        *(char *)(ctx + 0x85) = 1;
        if (*(float *)(ctx + 0x48) <= 0.0f) {
            flag = *(char *)(ctx + 0x86);
            if (flag == 1) {
                *(char *)(ctx + 0x86) = flag | 2;
                *(volatile float *)0x700038A0 = 0.0f;
                *(volatile float *)0x700038A4 = 0.0f;
                *(volatile float *)0x700038A8 = 5.0f;
                *(volatile float *)0x700038AC = 1.0f;
                func_001B2B10(self, D_700038A0, D_700038A0);
                func_001028B8(D_700038A0, self + 0xB0, D_700038A0);
                *(volatile float *)0x700038A4 = *(float *)(ctx + 0x60);
                func_001EFD90(0x80000016, D_700038A0, self + 0xC0);
                func_001E8B90(D_700038A0, 5.0f);
            }
            if (*(char *)(ctx + 0x80) & 2) {
                *(unsigned char *)(self + 6) = 7;
                *(char *)(ctx + 0x81) = -0x7F;
                *(float *)(ctx + 0x48) = 0.0f;
                *(int *)(ctx + 0x50) = 0;
                *(float *)(ctx + 0x4C) = 0.0f;
                anim_clip_init(self, 8, 0.0f, 0.0f);
                func_001FBD50(self, 0x7F9, 0, 300.0f);
            }
        }
        goto done;
    }
done:
    ;
}
