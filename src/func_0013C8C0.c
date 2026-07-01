// NEARMISS func_0013C8C0  (vram 0x0013C8C0, 0x488 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 93.30% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Fully recovered logic/structure using sibling func_0013C4C0 (98.98% NEARMISS) as the steering-family template. Residual at 93.3% (233) is a branch-likely-sense wall on the coin-flip dispatch: target emits chained beql/beqzl testing coin==1 then coin==0 (delay slots filled with the +/-pi/8 constan...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// AI steering/obstacle decision step for actor arg0, writing packed decision
// flags into its control block arg1+0x80/0x81 (same family as sibling
// func_0013C4C0/func_0013C1F0). Only runs the full probe when arg1[0x80]&0xF
// is currently clear (a "decision pending" gate).
//
// Probe 1: build a segment 25 units ahead along arg0's forward heading and
// call func_0013CD50(arg0, seg) -- an obstacle/edge classifier whose LOW BYTE
// result is stored directly into arg1[0x80]. If that byte is 0 (nothing
// found): probe a 20-unit segment rotated +/-pi/8 (D_3EC90FDB) about arg0's
// heading, alternating direction by a coin-flip on
// (D_70003B68 + D_70003B8A) & 1; func_0019AFE0 (or func_0013D220 for the
// first direction only) tests for a hit, and on a hit latches bit 1 (first
// direction) or bit 2 (second direction) into arg1[0x80].
// If probe 1's byte WAS nonzero (3 = "cornered"): compute a turn-frame index
// from arg1[0x80]'s low byte scaled through func_00122BB8 (a per-actor
// personality/aggression byte, bits [7:15]) into arg1+0x28/0x2A, then either
// steer toward the live player target (func_00102760 normalize, dot via
// func_00102738 against the ~135 degree cone 2.3561945f) picking a turn via
// func_001B1380, or (target too far off-angle) probe half the distance to
// the target (func_001B15D0) and pick a turn via func_001B39F0; either way
// the result (+1)*4 is OR'd into arg1[0x80].
// Tail: if arg1[0x80] has bit 0 or 1 set, run func_0013C4C0(arg0,arg1); if it
// has bit 2 or 3 set, run func_0013C1F0(arg0,arg1).
extern void func_001026A0(void *dst, void *a, void *b);
extern float func_00102738(void *a, void *b);
extern void func_00102760(void *dst, void *src);
extern void func_001028B8(void *dst, void *a, void *b);
extern void func_001029C0(void *a0);
extern void func_00102B08(void *obj, void *obj2, float v);
extern void func_00102BB0(void *obj, void *obj2, float v);
extern float func_0011E420(float a);
extern int func_00122BB8(signed char a0);
extern short func_00128250(float a0);
extern void func_0013C1F0(void *arg0, void *arg1);
extern void func_0013C4C0(void *arg0, void *arg1);
extern int func_0013CD50(void *arg0, void *seg);
extern int func_0013D220(void *arg0);
extern int func_0019AFE0(void *actor, void *from, void *to, int mask);
extern int func_001B1380(void *a, void *b, float c);
extern float func_001B15D0(void *a, void *b);
extern void func_001B2B10(void *a, void *b, void *c);
extern int func_001B39F0(void *arg0, void *arg1, void *arg2);

extern int D_700031B0;
extern float D_700036A0[];
extern float D_700038A0[];
extern float D_700038B0[];
extern float D_700038D0[];
extern int D_70003B68;
extern short D_70003B8A;

void func_0013C8C0(char *arg0, signed char *arg1) {
    signed char result;
    int coin;
    signed char turn;

    if (!(arg1[0x80] & 0xF)) {
        *(int *)0x700038A0 = 0;
        *(int *)0x700038A4 = 0;
        *(float *)0x700038A8 = 25.0f;
        *(int *)0x700038AC = 0x3F800000;
        func_001B2B10(D_700038A0, D_700038A0, D_700038A0);
        func_001028B8(D_700038A0, D_700038A0, arg0 + 0xB0);
        result = (signed char)func_0013CD50(arg0, D_700038A0);
        arg1[0x80] = result;
        if (result == 0) {
            func_001029C0(D_700036A0);
            *(int *)0x700038A0 = 0;
            *(int *)0x700038A4 = 0;
            *(float *)0x700038A8 = 20.0f;
            *(int *)0x700038AC = 0x3F800000;
            coin = (D_70003B68 + D_70003B8A) & 1;
            if (coin == 0) {
                func_00102B08(D_700036A0, D_700036A0, 0.39269909f);
                func_00102BB0(D_700036A0, D_700036A0, *(float *)(arg0 + 0xC4));
                func_001026A0(D_700038A0, D_700036A0, D_700038A0);
                func_001028B8(D_700038A0, D_700038A0, arg0 + 0xB0);
                if (func_0019AFE0(arg0, arg0 + 0xB0, D_700038A0, 6) != 0 || func_0013D220(arg0) != 0) {
                    arg1[0x80] = arg1[0x80] | 1;
                }
            } else {
                func_00102B08(D_700036A0, D_700036A0, -0.39269909f);
                func_00102BB0(D_700036A0, D_700036A0, *(float *)(arg0 + 0xC4));
                func_001026A0(D_700038A0, D_700036A0, D_700038A0);
                func_001028B8(D_700038A0, D_700038A0, arg0 + 0xB0);
                if (func_0019AFE0(arg0, arg0 + 0xB0, D_700038A0, 6) != 0) {
                    arg1[0x80] = arg1[0x80] | 2;
                }
            }
        } else if (result == 3) {
            *(short *)(arg1 + 0x28) = func_00128250(66.666664f * ((float)((func_00122BB8(result) >> 7) & 0xFF) / 255.0f));
            *(short *)(arg1 + 0x2A) = 0;
            *(int *)0x700038A0 = 0;
            *(int *)0x700038A4 = 0;
            *(float *)0x700038A8 = 1.0f;
            *(int *)0x700038AC = 0x3F800000;
            func_001B2B10(arg0, D_700038A0, D_700038A0);
            *(float *)0x700038B0 = *(float *)(*(int *)0x700031D0 + 0x24);
            *(int *)0x700038B4 = 0;
            *(float *)0x700038B8 = *(float *)(*(int *)0x700031D0 + 0x2C);
            *(int *)0x700038BC = 0x3F800000;
            func_00102760(D_700038B0, D_700038B0);
            if (func_0011E420(func_00102738(D_700038A0, D_700038B0)) <= 2.3561945f) {
                func_001028B8(D_700038B0, D_700038B0, arg0 + 0xB0);
                turn = (signed char)((func_001B1380(D_700038B0, arg0 + 0xB0, *(float *)(arg0 + 0xC4)) + 1) * 4);
            } else {
                *(int *)0x700038A8 = 0;
                *(int *)0x700038A4 = 0;
                *(int *)0x700038A0 = 0;
                *(int *)0x700038AC = 0x3F800000;
                *(float *)0x700038A8 = func_001B15D0(arg0 + 0xB0, &D_700031B0) / 2.0f;
                func_001B2B10(arg0, D_700038A0, D_700038A0);
                func_001028B8(D_700038A0, D_700038A0, arg0 + 0xB0);
                turn = (signed char)((func_001B39F0(arg0, D_700038A0, D_700038D0) + 1) * 4);
            }
            arg1[0x80] = turn;
        }
    }
    if (arg1[0x80] & 3) {
        func_0013C4C0(arg0, arg1);
    }
    if (arg1[0x80] & 0xC) {
        func_0013C1F0(arg0, arg1);
    }
}
