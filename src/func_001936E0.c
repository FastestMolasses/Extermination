// NEARMISS func_001936E0  (vram 0x001936E0, 0x6AC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.24% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful (matching wave 2026-10-27, was 94.99%):
// fixes: func_00193660 is called with (arg0, arg1); the near-hand angle nudge stores 30+yaw directly when above it, else 10+(yaw+reach) when below that; the sqrt distances and the case-1 angle are kept in scratchpad 0x70003A20/0x70003A24 (angle as a relocated extern); the second kind switch lists its cases in reverse (2.3.3 emits compares in reverse source order); `> 0x50` counter test.
// Remaining differences: register colouring only -- the state dispatch keeps the state byte in a0 with the constant in v1 (ours v1/v0, so case 0's first constant is not a branch-likely slot), and the case-1 tail uses v1 where ours uses v0. Neither local order, parameter copies, unsigned-char params, done placement nor compound assignments moved it.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Melee-grab / lunge-attack state machine for actor arg0 vs. target arg1.
// Dispatches on arg0+1 (sub-state): state 0 starts the attack -- resets
// +0xA0 timer to 0x78, clears +2/+8, stages the actor's grab-point
// transform (arg0+0x30 -> D_70003B50 -> D_70003400), then by the target's
// "kind" field (arg1+0x230; kinds 2/4/15 are the "small" targets) picks a
// reach distance (9.0f vs 19.0f) and builds a facing-offset delta into
// D_70003600 with a Y-drop equal to -(arg0+0x4C) via func_001031E0, then
// transforms it (func_001026A0) into arg0+0x10 as the swing target. It then
// blends arg0+0x14 (a wind-up angle) toward D_008105E0 and re-checks the
// target kind AGAIN: for kinds 2/4/15 it just tries to finish the grab
// (func_00193660) and resets on success; otherwise it measures the
// horizontal (x/z) distance from the swing target to the actual hand
// position (arg0+0x10 vs arg0+0x20) via the mula.s/madd.s FMA idiom, and if
// under 7.0 nudges arg0+0x14 toward a target angle 30 (or 10+reach) degrees
// ahead of the target's yaw. Either way it clamps arg0+0x14 into
// [arg0+0x50, arg0+0x54] and restages the animation. State 1 continues the
// grab: blends the swing/wind-up interpolants again, and if a "hold" timer
// (D_008105D4) is small enough, re-targets the hold point (D_008105D0/D8)
// toward the target's position using the mula.s/madd.s distance idiom and
// an angle blend (func_0011E620/func_001B1470); either way it advances the
// grab frame counter (+8), measures how close the hand delta
// (D_700038A0 = arg0+0x10 - D_008105D0) still is, and if within 0.25 (or the
// target is already falling, arg1+0x38>0, hurries the counter by 10) or the
// counter reaches 0x51, ends the grab (clears +1/+2/+3/+6). After the
// dispatch, func_00191210 runs once more, then a second switch on the
// target's kind (arg1+0x230): kinds 33/15/2/1 leave +1/+2/+3/+6 untouched;
// 41/12 set the "grab class" byte +6=2 and clear +1; 13/42 set +6=1 and
// clear +1; any other kind clears +6=0 and +1/+2/+3. Finally, if the grab
// class settled on 3 and the currently-held input
// bits match D_70003B80, it resets +1, snapshots arg1+0xC4 into +0x48, and
// derives a new wind-up rate (+0x4C) from fabsf(D_0081069C) clamped into
// [7.0, fabsf(arg0+0x64)], plus a blended aim angle at +0x44.
extern void func_001026A0(void *a0, void *a1, void *a2);
extern float func_00102738(void *a, void *b);
extern void func_001028D0(void *dst, void *a, void *b);
extern void func_00102948(void *dst, void *src);
extern void func_001029C0(void *m);
extern void func_00102C58(void *dst, void *a, void *b);
extern void func_001031E0(void *dst, void *src);
extern float func_0011DE90(float a);
extern float func_0011DF78(float a);
extern float func_0011E2A8(float a);
extern float func_0011E620(float a, float b);
extern float func_0011E748(float a);
extern void func_0018C4B0(void *dst, float target, float rate);
extern void func_0018C6A0(void *dst, void *src, float rate);
extern void func_0018D7B0(void *arg0, int a1);
extern void func_00191210(void);
extern int func_00193660(char *a0, char *a1);
extern float func_001B1240(void *p, float a, float b);
extern float func_001B1470(float a);

extern float D_008105D0;
extern float D_008105D4;
extern float D_008105D8;
extern float D_008105E0;
extern float D_008105E8;
extern float D_0081069C;
extern unsigned short D_00810E74;
extern char D_70003400[];
extern char D_70003430[];
extern char D_70003600[];
extern char D_70003630[];
extern char D_700038A0[];
extern char D_70003B50[];
extern float D_70003A24;

void func_001936E0(char *arg0, char *arg1) {
    unsigned char st;
    int done;
    int kind;
    float reach;
    float gx, gz;

    st = *(unsigned char *)(arg0 + 1);
    done = 0;

    switch (st) {
    case 0:
        *(short *)(arg0 + 0xA0) = 0x78;
        *(unsigned char *)(arg0 + 1) = *(unsigned char *)(arg0 + 1) + 1;
        *(char *)(arg0 + 2) = 0;
        *(short *)(arg0 + 8) = 0;
        func_00102948(arg0 + 0x30, D_70003B50);
        func_001029C0(D_70003400);
        func_00102C58(D_70003400, D_70003400, arg0 + 0x30);

        kind = *(int *)(arg1 + 0x230);
        switch (kind) {
        case 2:
        case 4:
        case 15:
            reach = 9.0f;
            func_00102948(arg0 + 0x20, arg1 + 0xA0);
            *(float *)(arg0 + 0x24) = 11.0f + (*(float *)(arg1 + 0xA4) + *(float *)(arg0 + 0x8C));
            func_001031E0(D_70003430, arg1 + 0xA0);
            *(float *)0x70003600 = 0.0f;
            *(int *)0x70003604 = 0x41100000;
            *(float *)0x70003608 = -*(float *)(arg0 + 0x4C);
            *(int *)0x7000360C = 0x3F800000;
            break;
        default:
            reach = 19.0f;
            func_00102948(arg0 + 0x20, arg1 + 0xA0);
            *(float *)(arg0 + 0x24) = 11.0f + (6.0f + *(float *)(arg1 + 0xA4));
            func_001031E0(D_70003430, arg1 + 0xA0);
            *(float *)0x70003600 = 0.0f;
            *(int *)0x70003604 = 0x41980000;
            *(float *)0x70003608 = -*(float *)(arg0 + 0x4C);
            *(int *)0x7000360C = 0x3F800000;
            break;
        }
        func_001026A0(arg0 + 0x10, D_70003400, D_70003600);
        func_0018C4B0(&D_008105E0, *(float *)(arg0 + 0x24), 2.0f);
        func_0018C6A0(arg0 + 0x20, &D_008105E0, 2.0f);
        func_0018D7B0(arg0, 6);

        kind = *(int *)(arg1 + 0x230);
        switch (kind) {
        case 2:
        case 4:
        case 15:
            if (func_00193660(arg0, arg1) != 0) {
                *(unsigned char *)(arg0 + 6) = 0;
                *(unsigned char *)(arg0 + 1) = 0;
                *(char *)(arg0 + 2) = 0;
                *(char *)(arg0 + 3) = 0;
            }
            break;
        default:
            func_001028D0(D_70003630, arg0 + 0x10, arg0 + 0x20);
            if (func_0011E748(*(float *)0x70003630 * *(float *)0x70003630 + *(float *)0x70003638 * *(float *)0x70003638) < 7.0f) {
                float cur = *(float *)(arg0 + 0x14);
                float ang = 30.0f + *(float *)(arg1 + 0xA4);
                if (!(cur <= ang)) {
                    *(float *)(arg0 + 0x14) = ang;
                } else {
                    ang = 10.0f + (*(float *)(arg1 + 0xA4) + reach);
                    if (cur < ang) {
                        *(float *)(arg0 + 0x14) = ang;
                    }
                }
            }
            break;
        }

        if (*(float *)(arg0 + 0x14) < *(float *)(arg0 + 0x50)) {
            *(float *)(arg0 + 0x14) = *(float *)(arg0 + 0x50);
        }
        if (!(*(float *)(arg0 + 0x14) <= *(float *)(arg0 + 0x54))) {
            *(float *)(arg0 + 0x14) = *(float *)(arg0 + 0x54);
        }
        func_0018D7B0(arg0, 6);
        func_00191210();
        break;

    case 1:
        func_0018C4B0(&D_008105E0, *(float *)(arg0 + 0x24), 2.0f);
        func_0018C6A0(arg0 + 0x20, &D_008105E0, 2.0f);
        func_0018C6A0(arg0 + 0x10, &D_008105D0, 4.0f);
        func_0018C4B0(&D_008105D0, *(float *)(arg0 + 0x14), 4.0f);

        if (D_008105D4 < (23.0f + *(float *)(arg1 + 0xA4))) {
            gx = D_008105D0 - *(float *)(arg1 + 0xA0);
            gz = D_008105D8 - *(float *)(arg1 + 0xA8);
            *(float *)0x70003A20 = func_0011E748(gx * gx + gz * gz);
            if (*(float *)0x70003A20 < 8.0f) {
                D_70003A24 = func_001B1470(func_0011E620(gx, gz));
                D_008105D0 = *(float *)(arg1 + 0xA0) + (8.0f * func_0011E2A8(D_70003A24));
                D_008105D8 = *(float *)(arg1 + 0xA8) + (8.0f * func_0011DE90(D_70003A24));
            }
        }

        *(short *)(arg0 + 8) = *(short *)(arg0 + 8) + 1;
        func_001028D0(D_700038A0, arg0 + 0x10, &D_008105D0);
        *(float *)0x70003A20 = func_0011E748(func_00102738(D_700038A0, D_700038A0));
        if (*(float *)0x70003A20 < 0.25f) {
            done = 1;
        }
        if (!(*(float *)(arg1 + 0x38) <= 0.0f)) {
            *(short *)(arg0 + 8) = *(short *)(arg0 + 8) + 0xA;
        }
        if (done != 0 || *(short *)(arg0 + 8) > 0x50) {
            *(unsigned char *)(arg0 + 6) = 0;
            *(unsigned char *)(arg0 + 1) = 0;
            *(char *)(arg0 + 2) = 0;
            *(char *)(arg0 + 3) = 0;
        }
        break;
    }

    func_00191210();

    switch (*(int *)(arg1 + 0x230)) {
    case 0x2A:
    case 0xD:
        *(unsigned char *)(arg0 + 6) = 1;
        *(unsigned char *)(arg0 + 1) = 0;
        break;
    case 0xC:
    case 0x29:
        *(unsigned char *)(arg0 + 6) = 2;
        *(unsigned char *)(arg0 + 1) = 0;
        break;
    case 1:
    case 2:
    case 0xF:
    case 0x21:
        /* untouched -- falls straight to the tail check below */
        break;
    default:
        *(unsigned char *)(arg0 + 6) = 0;
        *(unsigned char *)(arg0 + 1) = 0;
        *(char *)(arg0 + 2) = 0;
        *(char *)(arg0 + 3) = 0;
        break;
    }

    if (*(unsigned char *)(arg0 + 6) == 3 && (D_00810E74 & *(unsigned short *)0x70003B80)) {
        float rate;
        *(unsigned char *)(arg0 + 1) = 0;
        *(float *)(arg0 + 0x48) = *(float *)(arg1 + 0xC4);
        rate = func_0011DF78(D_0081069C);
        *(float *)(arg0 + 0x4C) = rate;
        if (rate < 7.0f) {
            *(float *)(arg0 + 0x4C) = 7.0f;
        } else if (!(*(float *)(arg0 + 0x4C) <= func_0011DF78(*(float *)(arg0 + 0x64)))) {
            *(float *)(arg0 + 0x4C) = func_0011DF78(*(float *)(arg0 + 0x64));
        }
        *(float *)(arg0 + 0x44) = func_001B1240(&D_008105D0, D_008105E0, D_008105E8);
    }
}
