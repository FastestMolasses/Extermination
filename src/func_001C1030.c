// NEARMISS func_001C1030  (vram 0x001C1030, 0x4D0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 95.24% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation-order permutation (arg0 copy colored to $s0 vs target's $s1) plus one operand-order/sign-extend scheduling artifact in the state-1/sub-state-0 timer decrement (target computes `field34 - (v36&0xFFF)` via a reversed subu operand order with an extra dsll32/dsra32 64-bit sign-ext...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Actor animation/AI state machine. State 0: registers the actor via
// func_001B10B0(arg0, 0x26, 0x27) (bails if already registered), inits bones,
// starts anim clip 1, marks itself active (state=1), and seeds two scratch
// VU0 transforms (D_700036A0 for facing, D_700038A0 group for an orbit/patrol
// waypoint) used to precompute two initial "time to reach" estimates
// (arg0+0x60/0x64) via func_0011E748 (sqrt of dx^2+dy^2 against the fixed
// camera-like reference D_700031B0/B4/B8), stored /20 as a per-frame step.
// State 1: sub-state 0 decrements a countdown (arg0+0x34) by the low 12 bits
// of a request word (arg0+0x36) when its 0x1000 flag is set, transitioning to
// state 2 (either immediately if the countdown expires, or via a "turning"
// sub-state 1 first); sub-state 1 seeds a 0x14-tick turn count and falls
// into sub-state 2, which decrements a heading angle (arg0+0x38) by fixed
// steps, rebuilds a rotation matrix (D_70003400 group) each tick, and reverts
// to state 1 sub-state 0 once the turn count hits zero. All sub-states end by
// advancing the clip, updating the transform (func_001C68C0/func_001B17A0)
// and invoking the actor's per-frame hook at +0x4C. State 2: sub-state 0
// starts a "reached" clip and zeroes a scratch flags field (arg0+0x1F0),
// firing a fixed 3D sound cue; sub-state 1 waits for that scratch field's
// 0x1000 completion flag to advance to state 3 (done). State 3: notifies
// func_001B1190 with the actor's area-linkage byte (+0x9A) and frees the
// actor via func_001AFC10.
extern int anim_advance_time(int self, float t);
extern void anim_clip_init(char *self, int clip, float a, float b);
extern void bone_init_default_2(char *actor, int a1);
extern void func_001026A0(void *a0, void *a1, void *a2);
extern void func_00102948(void *a0, void *a1);
extern void func_001029C0(void *a0);
extern void func_00102BB0(void *a0, void *a1, float a2);
extern float func_0011E2A8(float x);
extern float func_0011E748(float x);
extern int func_0019A570(char *a0, void *a1, int a2, int a3);
extern void func_001AFC10(char *actor);
extern int func_001B10B0(char *arg0, int arg1, int arg2);
extern void func_001B1190(int a0);
extern void func_001B17A0(char *self);
extern int func_001C6160(int *a0);
extern void func_001C68C0(char *o);
extern void func_001D8BF0(void *a0, int a1);
extern void func_001FB9F0(int a0, int a1, int a2, int a3);

extern float D_700031B0;
extern float D_700031B4;
extern float D_700031B8;
extern float D_70003400[4];
extern float D_70003430[4];
extern float D_70003600[4];
extern float D_700036A0[4];
extern float D_700036D0[4];
extern float D_70003800[4];
extern float D_700038A0;
extern float D_70003A20;
extern float D_70003A24;

void func_001C1030(char *arg0) {
    unsigned char state5;
    short *scratch1f0 = (short *)(arg0 + 0x1F0);

    switch (*(unsigned char *)(arg0 + 4)) {
    case 0:
        if (func_001B10B0(arg0, 0x26, 0x27) != 0) {
            return;
        }
        bone_init_default_2(arg0, 1);
        anim_clip_init(arg0, 1, 0.0f, (float)func_001C6160((int *)arg0));
        *(short *)(arg0 + 0x34) = 1;
        *(char *)(arg0 + 0) = 1;
        *(unsigned char *)(arg0 + 4) = 1;
        func_00102948(arg0 + 0xA0, arg0 + 0xB0);
        func_001D8BF0(arg0, 1);
        func_001029C0(D_700036A0);
        func_00102BB0(D_700036A0, D_700036A0, *(float *)(arg0 + 0xC4));
        func_00102948(D_700036D0, arg0 + 0xA0);
        *(float *)0x700038A0 = 40.0f;
        *(float *)0x700038A4 = 0.0f;
        *(float *)0x700038A8 = 0.0f;
        *(float *)0x700038AC = 1.0f;
        func_001026A0(&D_700038A0, D_700036A0, &D_700038A0);
        if (func_0019A570(arg0 + 0xB0, &D_700038A0, 4, 0) != 0) {
            *(float *)0x700038A0 = D_700031B0 - *(float *)(arg0 + 0xB0);
            *(float *)0x700038A8 = D_700031B8 - *(float *)(arg0 + 0xB8);
            D_70003A20 = func_0011E748(*(float *)0x700038A0 * *(float *)0x700038A0 + *(float *)0x700038A8 * *(float *)0x700038A8);
            *(float *)(arg0 + 0x60) = D_70003A20 / 20.0f;
        }
        func_00102948(&D_700038A0, arg0 + 0xB0);
        *(float *)0x700038A4 = *(float *)0x700038A4 - 40.0f;
        if (func_0019A570(arg0 + 0xB0, &D_700038A0, 4, 0) != 0) {
            float dy = D_700031B4 - *(float *)(arg0 + 0xB4);
            D_70003A20 = dy;
            D_70003A20 = func_0011E748(dy * dy);
            *(float *)(arg0 + 0x64) = D_70003A20 / 20.0f;
            return;
        }
        break;
    case 1:
        state5 = *(unsigned char *)(arg0 + 5);
        switch (state5) {
        case 0:
        {
            short v36 = *(short *)(arg0 + 0x36);
            if (v36 != 0) {
                if (v36 & 0x1000) {
                    *(short *)(arg0 + 0x34) = *(short *)(arg0 + 0x34) - (v36 & 0xFFF);
                    if (*(short *)(arg0 + 0x34) <= 0) {
                        *(char *)(arg0 + 0) = 2;
                        *(unsigned char *)(arg0 + 4) = 2;
                        *(unsigned char *)(arg0 + 5) = 0;
                    } else {
                        *(char *)(arg0 + 0) = 2;
                        *(short *)(arg0 + 0x36) = 0;
                        *(unsigned char *)(arg0 + 5) = 1;
                    }
                } else {
                    *(short *)(arg0 + 0x36) = 0;
                    *(char *)(arg0 + 0) = 2;
                    *(unsigned char *)(arg0 + 5) = 1;
                }
            }
            break;
        }
        case 1:
            func_00102948(arg0 + 0xB0, arg0 + 0xA0);
            *(short *)(arg0 + 0x28) = 0x14;
            *(float *)(arg0 + 0x38) = 0.0f;
            *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) + 1;
            /* fallthrough */
        case 2:
            *(float *)(arg0 + 0x38) = *(float *)(arg0 + 0x38) - 0.31415927f;
            *(float *)0x70003600 = 0.0f;
            *(float *)0x70003604 = 0.0f;
            *(float *)0x70003608 = 0.5f * func_0011E2A8(*(float *)(arg0 + 0x38));
            *(float *)0x7000360C = 1.0f;
            func_001029C0(D_70003400);
            func_00102BB0(D_70003400, D_70003400, *(float *)(arg0 + 0xC4));
            func_00102948(D_70003430, arg0 + 0xA0);
            func_001026A0(arg0 + 0xB0, D_70003400, D_70003600);
            *(short *)(arg0 + 0x28) = *(short *)(arg0 + 0x28) - 1;
            if (*(short *)(arg0 + 0x28) == 0) {
                *(char *)(arg0 + 0) = 1;
                *(unsigned char *)(arg0 + 5) = 0;
                func_00102948(arg0 + 0xB0, arg0 + 0xA0);
            }
            break;
        }
        anim_advance_time((int)arg0, 1.0f);
        func_001C68C0(arg0);
        func_001B17A0(arg0);
        (*(void (**)(char *))(arg0 + 0x4C))(arg0);
        return;
    case 2:
        state5 = *(unsigned char *)(arg0 + 5);
        switch (state5) {
        case 0:
            anim_clip_init(0, 2, 0.0f, 0.0f);
            *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) + 1;
            *scratch1f0 = 0;
            func_001FB9F0(0x442, 0x1000, 0x1000, 0x1000);
            break;
        case 1:
            if (*scratch1f0 & 0x1000) {
                *(unsigned char *)(arg0 + 4) = 3;
            }
            break;
        }
        anim_advance_time((int)arg0, 1.0f);
        func_001C68C0(arg0);
        (*(void (**)(char *))(arg0 + 0x4C))(arg0);
        return;
    case 3:
        func_001B1190(*(unsigned char *)(arg0 + 0x9A));
        func_001AFC10(arg0);
        break;
    }
}
