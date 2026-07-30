// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Five-state effect/attack driver for actor `self` against owner context `ctx`
// (state byte at self+6): 0 arms the effect (rate 2.0, flag ctx+0x88) and kicks
// clip 0x10; 1 waits for the owner's 0x1000 input bit, then rate 1.0 + clip 0x11;
// 2 fires cue 0x7E2 at pose 7.0, eases the heading (self+0xC4) toward the aim
// angle from func_001B1240, and — when the target probe (func_0021BED0 /
// func_0014C130 / |D_00810354 - self+0xB4| <= 6.0) passes — transforms the four
// D_00245B90 probe quads into world space and tests them with func_001B1EA0;
// a hit advances the state, registers the strike (func_0021BF90), publishes the
// recoil angle to D_00810374, builds the (-0.5, 0, 17.15, 1.0) impact vector in
// scratchpad and kicks clip 0x12, a miss falls back to state 4 + clip 0x14;
// 3 runs the pose-14.0 finisher: a (1.5, 15.0, 15.0, 1.0) scratchpad vector is
// transformed, spun by the heading, copied to 0x700036D0, spawned via
// func_001EFEB0, the camera distances (D_008104D4 / D_008104DC) are picked from
// the zoom flag self+0xD:0x80 and D_0081070A, then the hit list is flushed and
// clip 0x13 kicked; 4 clears the state on the next input bit.
//
// Three source shapes are load-bearing for the byte match:
//  * the 0x40-byte probe template copy is a single `struct S64` (four 128-bit
//    members) assignment — that is what yields CodeWarrior's batched
//    lq t0/a3/a2/a1 + sq block with the dest pointer staged in t1; four explicit
//    128-bit temps instead colour the loads in the opposite direction.
//  * every early exit is `goto done` to a single trailing label (idiom-27): the
//    shared-exit CFG stops mwcc 2.3.3 speculating constant materialisations into
//    the switch-dispatch and guard branch delay slots, restoring the unfilled
//    NOPs and the dead `addiu v0, a1, 1` re-materialisation. Likewise the
//    D_008104DC store is written inside all four camera-distance arms rather
//    than hoisted to the join.
//  * `zi = 0; z = (float)zi;` (idiom-24) at the clip-0x12 call, and the
//    `float f0..f3` block declared BEFORE the four `volatile float` scratchpad
//    reads: the volatile keeps the loads in address order while the separate
//    declaration hands out $f3..$f0 top-down, which is CodeWarrior's colouring.

extern void anim_clip_init(char *self, int clip, float speed, float d);
extern float func_001B1240(float *obj, float px, float py);
extern float func_001B12B0(float goal, float cur, float rate);
extern float func_001B1470(float v);
extern float func_0011DF78(float a0);
extern int func_0014C130(char *self);
extern int func_0021BED0(unsigned char *a0);
extern void func_0021BF90(unsigned char *a0, void *a1);
extern void func_0021C040(unsigned char *a0, void *a1);
extern int func_001B1EA0(int a, void *b, void *c, int d);
extern void func_001B2B10(char *self, void *dst, void *src);
extern void func_001028B8(void *a, void *b, void *c);
extern void func_001029C0(void *a0);
extern void func_00102BB0(void *a0, void *a1, float f12);
extern void func_00182F90(unsigned char *p, void *a1);
extern void *func_001EFEB0(int a0, void *a1);
extern int func_001FBD50(void *a0, int a1, int a2, float f12);

extern int D_00245B90[16];
extern unsigned char D_008102B0;
extern float D_00810354;
extern float D_00810360;
extern float D_00810368;
extern float D_00810374;
extern int D_008104D4;
extern int D_008104DC;
extern unsigned char D_008106BD;
extern unsigned char D_0081070A;
extern int D_700036A0[4];
extern int D_700038A0[4];

typedef unsigned __int128 uint128;
struct S64 { uint128 a, b, c, d; };

void func_00149B50(char *self, char *ctx) {
    int probe[16];
    int state;
    int zi;
    float z;
    *(struct S64 *)probe = *(struct S64 *)D_00245B90;

    state = *(unsigned char *)(self + 6);
    switch (state) {
    case 0:
        *(unsigned char *)(self + 6) = state + 1;
        *(int *)(ctx + 0x40) = 0x40000000;
        *(char *)(ctx + 0x83) = 0;
        *(char *)(ctx + 0x88) = 1;
        *(int *)(ctx + 0x50) = 0;
        *(int *)(ctx + 0x4C) = 0;
        anim_clip_init(self, 0x10, 5.0f, (z = 0.0f));
        goto done;
    case 1:
        if (*(int *)(ctx + 0x20) & 0x1000) {
            *(unsigned char *)(self + 6) = state + 1;
            *(int *)(ctx + 0x40) = 0x3F800000;
            z = 0.0f;
            anim_clip_init(self, 0x11, z, z);
            goto done;
        }
        goto done;
    case 2:
        if (*(float *)(self + 0x3C) == 7.0f) {
            func_001FBD50(self, 0x7E2, 0, 300.0f);
        }
        *(float *)(ctx + 0x44) = func_001B1240((float *)(self + 0xB0), D_00810360, D_00810368);
        *(float *)(self + 0xC4) = func_001B12B0(*(float *)(ctx + 0x44), *(float *)(self + 0xC4), 0.0698131695f);
        if (*(int *)(ctx + 0x20) & 0x1000) {
            if (func_0021BED0(&D_008102B0) == 0
                && func_0014C130(self) != 0
                && func_0011DF78(D_00810354 - *(float *)(self + 0xB4)) <= 6.0f) {
                int i = 0;
                int *p = probe;
                do {
                    func_001B2B10(self, p, p);
                    func_001028B8(p, p, self + 0xB0);
                    i += 1;
                    p += 4;
                } while (i < 4);
                if (func_001B1EA0(0, &D_00810360, probe, 4) != 0) {
                    *(unsigned char *)(self + 6) = *(unsigned char *)(self + 6) + 1;
                    func_0021BF90(&D_008102B0, self);
                    *(char *)(ctx + 0x86) = 1;
                    D_00810374 = func_001B1470(3.1415927f + *(float *)(self + 0xC4));
                    *(int *)0x700038A0 = 0xBF000000;
                    *(int *)0x700038A4 = 0;
                    *(int *)0x700038A8 = 0x41893333;
                    *(int *)0x700038AC = 0x3F800000;
                    func_001B2B10(self, D_700038A0, D_700038A0);
                    func_001028B8(D_700038A0, D_700038A0, self + 0xB0);
                    func_00182F90(&D_008102B0, D_700038A0);
                    zi = 0;
                    z = (float)zi;
                    anim_clip_init(self, 0x12, 1.0f, z);
                    goto done;
                }
            }
            *(unsigned char *)(self + 6) = 4;
            anim_clip_init(self, 0x14, 1.0f, (z = 0.0f));
            goto done;
        }
        break;
    case 3:
        if (*(float *)(self + 0x3C) == 14.0f) {
            *(int *)0x700038A0 = 0x3FC00000;
            *(int *)0x700038A4 = 0x41700000;
            *(int *)0x700038A8 = 0x41700000;
            *(int *)0x700038AC = 0x3F800000;
            func_001B2B10(self, D_700038A0, D_700038A0);
            func_001028B8(D_700038A0, self + 0xB0, D_700038A0);
            func_001029C0(D_700036A0);
            func_00102BB0(D_700036A0, D_700036A0, *(float *)(self + 0xC4));
            {
                float f0, f1, f2, f3;
                f3 = *(volatile float *)0x700038A0;
                f2 = *(volatile float *)0x700038A4;
                f1 = *(volatile float *)0x700038A8;
                f0 = *(volatile float *)0x700038AC;
                *(float *)0x700036D0 = f3;
                *(float *)0x700036D4 = f2;
                *(float *)0x700036D8 = f1;
                *(float *)0x700036DC = f0;
            }
            func_001EFEB0(0x8000002A, D_700036A0);
            if (*(unsigned char *)(self + 0xD) & 0x80) {
                if (D_0081070A != 0) {
                    D_008104D4 = 0x41C80000;
                    D_008104DC = 0x42340000;
                } else {
                    D_008104D4 = 0x41A00000;
                    D_008104DC = 0x42200000;
                }
            } else if (D_0081070A != 0) {
                D_008104D4 = 0x41A00000;
                D_008104DC = 0x420C0000;
            } else {
                D_008104D4 = 0x41900000;
                D_008104DC = 0x420C0000;
            }
            D_008102B0 |= 2;
            *(char *)(ctx + 0x86) = 0;
            func_0021C040(&D_008102B0, self);
            if (D_008106BD == 1) {
                D_008106BD = 0;
            }
        }
        if (*(int *)(ctx + 0x20) & 0x1000) {
            *(unsigned char *)(self + 6) = *(unsigned char *)(self + 6) + 1;
            anim_clip_init(self, 0x13, 1.0f, 0.0f);
            goto done;
        }
        break;
    case 4:
        if (*(int *)(ctx + 0x20) & 0x1000) {
            *(char *)(self + 5) = 0;
            *(unsigned char *)(self + 6) = 0;
            *(char *)(ctx + 0x88) = 0;
            *(int *)(ctx + 0x40) = 0x3F800000;
        }
        break;
    }
done:
    ;
}
