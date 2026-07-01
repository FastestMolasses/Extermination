// NEARMISS func_0014C680  (vram 0x0014C680, 0x348 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.03% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Body/structure fully recovered (readable plain C matching the func_0014C9D0 sibling idiom exactly: same 0x700038A0/0x700038B0/0x700036A0/0x70003A20 VU0-scratchpad blend layout, same func_00102948/func_001B15D0/func_0011DF78/func_0011E620/func_001029C0/func_00102B08/func_001026A0 call chain). Resi...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// State-byte (arg0+6) driven proximity/idle timer, sibling of func_0014C9D0
// (same VU0-scratchpad recoil/blend idiom, same 0x700038A0/0x700038B0/
// 0x700036A0/0x70003A20 scratch layout). arg0 = entity, arg1 = linked block.
//
// state 0 (one-time init, falls through to state 1): advance state, set
// arg1+0x2C = 1.88f. Compute the delta between the player position
// (D_700038A0 <- arg0+0xB0) and a cached point (D_700038B0 <- D_00810360)
// against a ramped timer at scratch 0x700038B4 (+10.0f per tick); once the
// cached-vs-live compare crosses, derive an angle via func_0011E620 and, if
// negative, re-blend a target vector through func_001026A0 and stash the
// result back to arg1+0x24.
//
// state 1 body: tick down arg1+0x14 (a countdown), else reroll it to a
// random 0-15 value via func_00122BB8() and refresh the aim pose
// (func_001B1240 -> arg1+0x34, func_001B12B0 -> arg0+0xC4). If arg1+0x28
// (the perturbation) is still non-negative: probe the distance between
// arg0+0xB0 and D_00810360 (func_00102948 x2 + func_001B15D0) against a
// 6.5f early-out or a func_001B1560 proximity check at 80.0 degrees
// (1.3962635f); on failure of both, roll a fresh perturbation into
// arg1+0x28 from func_00122BB8().
//
// objdiff 98.03% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0); pinned
// 991202 = 94.6%. Body/structure fully recovered. Sole residual: an instruction-scheduling /
// operand-compute-order permutation on (1) the very first func_00102948(&D_700038A0,
// arg0+0xB0) call and (2) the cached/ramp load-add-store-compare sequence (target inserts an
// extra mov.s no-op and defers the 0x700038B4 store into the branch delay slot).

extern void func_001026A0(float *a, float *b, float *c);
extern void func_00102948(float *a, float *b);
extern void func_001029C0(float *a);
extern void func_00102B08(float *a, float *b, float v);
extern float func_0011DF78(float a);
extern float func_0011E620(float a, float b);
extern int func_00122BB8(void);
extern float func_001B1240(char *p, float a, float b);
extern float func_001B12B0(float a, float b, float c);
extern int func_001B1560(char *p, float *v, float r);
extern float func_001B15D0(float *a, float *b);

extern float D_00810360;
extern float D_00810368;
extern float D_700036A0;
extern float D_700038A0;
extern float D_700038B0;

void func_0014C680(char *arg0, char *arg1) {
    unsigned char st;
    int cnt;
    float cached, ramp, ang;
    float *p;

    st = *(unsigned char *)(arg0 + 6);
    switch (st) {
    case 0:
        *(unsigned char *)(arg0 + 6) = st + 1;
        *(float *)(arg1 + 0x2C) = 1.88f;
        func_00102948(p = &D_700038A0, (float *)(arg0 + 0xB0));
        func_00102948(&D_700038B0, &D_00810360);
        cached = *(volatile float *)0x700038A4;
        ramp = *(volatile float *)0x700038B4 + 10.0f;
        *(volatile float *)0x700038B4 = ramp;
        if (!(cached < ramp)) {
            break;
        }
        *(volatile float *)0x70003A20 = func_0011DF78(cached - ramp);
        *(volatile int *)0x700038B4 = 0;
        *(volatile int *)0x700038A4 = 0;
        *(volatile float *)0x70003A24 = func_001B15D0(&D_700038A0, &D_700038B0);
        ang = -1.0f * func_0011E620(*(volatile float *)0x70003A20, *(volatile float *)0x70003A24);
        *(volatile float *)0x70003A20 = ang;
        if (!(ang < 0.0f)) {
            break;
        }
        func_001029C0(&D_700036A0);
        func_00102B08(&D_700036A0, &D_700036A0, *(volatile float *)0x70003A20);
        *(volatile int *)0x700038A0 = 0;
        *(volatile int *)0x700038A4 = 0;
        *(volatile float *)0x700038A8 = 1.88f;
        *(volatile float *)0x700038AC = 1.0f;
        func_001026A0(&D_700038A0, &D_700036A0, &D_700038A0);
        *(float *)(arg1 + 0x24) = *(volatile float *)0x700038A4;
        break;
    case 1:
        cnt = *(int *)(arg1 + 0x14);
        if (cnt != 0) {
            *(int *)(arg1 + 0x14) = cnt - 1;
        } else {
            *(int *)(arg1 + 0x14) = (func_00122BB8() >> 15) & 0xF;
            *(float *)(arg1 + 0x34) = func_001B1240(arg0 + 0xB0, D_00810360, D_00810368);
            *(float *)(arg0 + 0xC4) = func_001B12B0(*(float *)(arg1 + 0x34), *(float *)(arg0 + 0xC4), 0.07853981852531433f);
        }
        if (*(float *)(arg1 + 0x28) < 0.0f) {
            break;
        }
        func_00102948(p = &D_700038A0, (float *)(arg0 + 0xB0));
        func_00102948(&D_700038B0, &D_00810360);
        *(volatile float *)0x70003A20 = func_0011DF78(*(volatile float *)0x700038A4 - *(volatile float *)0x700038B4);
        *(volatile int *)0x700038B4 = 0;
        *(volatile int *)0x700038A4 = 0;
        if (*(volatile float *)0x70003A20 <= 6.5f) {
            goto check_angle;
        }
        if (func_001B15D0(&D_700038A0, &D_700038B0) <= 100.0f) {
            goto reroll;
        }
    check_angle:
        if (func_001B1560(arg0, &D_700038B0, 1.3962634801864624f) != 0) {
            break;
        }
    reroll:
        *(float *)(arg1 + 0x28) = (-0.01f * ((float)((func_00122BB8() >> 11) & 0xFF) / 255.0f)) - 0.035f;
        break;
    }
}
