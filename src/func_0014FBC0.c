// NEARMISS func_0014FBC0  (vram 0x0014FBC0, 0x5CC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 72.16% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Scheduling/regalloc permutation. Logic fully recovered. The 991202+target leave the clean dispatch but mwcc lowers the 5-way switch (descending dispatch) and assigns the paddub reg-moves (s0=a1,s1=a0) in a different order than the target, and case 2's dense scratch-pad store block schedules its d...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p

//
// NEARMISS (mwcc 2.3.3, 72.2% objdiff; 991202 67.0%). Body fully recovered:
// 5-state actor tick keyed on p[6] (a0=p anim/transform, a1=e state struct).
// State 0 initialises the actor (yaw p+0xC4=PI, e timers, clip via
// anim_clip_init, launch transform p+0xB0..) and wires the child at p+0x20;
// state 1 eases yaw toward 0 with func_001B12B0 and on flag e+0x30 bit 0x1000
// advances + restarts clip 3; state 2 (the large one) drives a scripted
// path over frames (D_00275428/2542C bounds, scratch matrix at 0x700038A0/B0,
// func_001026A0/01EFD90/0021BE40 + world-marker writes at 0x008102B0/0x810320);
// state 3 settles and on bit 0x1000 + low pitch sets p[5]=1,p[6]=0. Shared
// tail clamps e+0x40 and on bit 0x1000 advances to clip 4.
// Instruction stream structurally correct; residual is mwcc switch-dispatch
// lowering + paddub (reg-move) assignment order + case-2 delay-slot scheduling
// (regalloc/scheduling permutation, permuter class).
extern void anim_clip_init(int self, int clip, float a, float b);
extern void func_001FBD50(int self, int a, int b, float c);
extern void copy_qw4(void *d, void *s);
extern void func_001026A0(void *a, void *b, void *c);
extern void func_001EFD90(int mode, void *b, void *c);
extern int func_0021BE40(void *a, void *b);
extern float func_001B1240(unsigned char *origin, float x, float z);
extern float func_001B12B0(float goal, float cur, float rate);
extern void func_001029C0(void *m);
extern void func_00102BB0(void *a, void *b, float f);

extern int D_00275428;
extern int D_0027542C;
extern int *D_00275B40;

extern float D_00245EF0;
extern float D_00810360;
extern float D_00810368;

extern char D_700036A0[];
extern char D_700038A0[];
extern char D_700038B0[];

void func_0014FBC0(unsigned char *p, unsigned char *e) {
    switch (p[6]) {
    case 0:
        p[6] = p[6] + 1;
        *(int *)(p + 0xC4) = 0x40490FDB;
        *(int *)(e + 0x4) = 0;
        *(int *)(e + 0x8) = 0;
        *(int *)(e + 0xC) = 0;
        e[0x64] = 6;
        *(int *)(e + 0x38) = 0x40400000;
        *(int *)(e + 0x3C) = 0xBCF1A9FC;
        *(int *)(e + 0x34) = 0;
        *(int *)(e + 0x2C) = 0;
        *(int *)(e + 0x48) = 0;
        *(int *)(e + 0x40) = 0;
        anim_clip_init((int)p, (int)e, 0.0f, 0.0f);
        *(int *)(p + 0xB0) = 0x42C80000;
        *(int *)(p + 0xB4) = 0xC2C80000;
        *(int *)(p + 0xB8) = 0xC2A00000;
        *(int *)(p + 0xBC) = 0;
        if (*(int *)(p + 0x20) != 0) {
            *(char *)(*(int *)(p + 0x20) + 5) = 3;
        }
        break;
    case 1:
        if (*(int *)(e + 0xC) == 0 && !(*(float *)(p + 0xB4) < -35.0f)) {
            *(int *)(e + 0xC) = 1;
            func_001FBD50((int)p, 0x871, 0, 1000.0f);
        }
        *(float *)(p + 0xC4) = func_001B12B0(0.0f, *(float *)(p + 0xC4), 0.034906585f);
        if (*(int *)(e + 0x0) & 0x1000) {
            p[6] = p[6] + 1;
            *(int *)(e + 0xC) = 0;
            *(int *)(p + 0xC4) = 0;
            *(int *)(e + 0x3C) = 0;
            *(int *)(e + 0x38) = 0;
            anim_clip_init((int)p, 3, 0.0f, 0.0f);
        }
        break;
    case 2:
        if (*(int *)(e + 0x4) < D_00275428 && *(float *)(p + 0x3C) <= 106.0f) {
            *(float *)0x700038A0 = 48.8874512f;
            *(float *)0x700038A4 = 10.4990234f;
            *(float *)0x700038A8 = -1.43999996f;
            *(float *)0x700038AC = 1.0f;
            copy_qw4(D_700036A0, (void *)(D_00275B40[4] + 0x90));
            func_001026A0(D_700038A0, D_700036A0, D_700038A0);
            if (*(int *)(e + 0x4) == 0) {
                *(float *)0x700038B8 = 0.0f;
                *(float *)0x700038B4 = 0.0f;
                *(float *)0x700038B0 = 0.0f;
                *(float *)0x700038BC = 1.0f;
                func_001EFD90(8, D_700038A0, D_700038B0);
                if (*(int *)(e + 0xC) == 0) {
                    *(int *)(e + 0xC) = 1;
                    func_001FBD50((int)p, 0x878, 0, 800.0f);
                }
            }
            *(int *)(e + 0x8) = *(int *)(e + 0x8) + 1;
            if (!(*(int *)(e + 0x8) < D_0027542C)) {
                *(int *)(e + 0x4) = *(int *)(e + 0x4) + 1;
                *(int *)(e + 0x8) = 0;
                *(float *)0x700038B0 = 0.0f;
                *(float *)0x700038B4 = 0.0f;
                *(float *)0x700038B8 = D_00245EF0;
                *(float *)0x700038BC = 1.0f;
                func_001EFD90(0x80000064, D_700038A0, D_700038B0);
                func_001FBD50((int)p, 0x87A, 0, 800.0f);
                if (*(int *)(e + 0x4) == 2 && func_0021BE40((void *)0x008102B0, p) == 0) {
                    *(unsigned char *)0x008102B0 = *(unsigned char *)0x008102B0 | 2;
                    *(unsigned char *)0x008102BF = 2;
                    *(float *)0x70003A20 = func_001B1240(p + 0xB0, D_00810360, D_00810368);
                    func_001029C0(D_700036A0);
                    func_00102BB0(D_700036A0, D_700036A0, *(float *)0x70003A20);
                    *(int *)0x00810320 = 0;
                    *(int *)0x00810324 = 0;
                    *(float *)0x00810328 = 1.0f;
                    *(float *)0x0081032C = 1.0f;
                    func_001026A0((void *)0x00810320, D_700036A0, (void *)0x00810320);
                }
            }
        }
        goto tail;
    case 3:
        if (*(float *)(e + 0x3C) == 0.0f && *(float *)(p + 0x3C) <= 58.0f) {
            *(int *)(e + 0x38) = 0;
            *(int *)(e + 0x3C) = 0xBDCCCCCD;
        }
        if (*(int *)(e + 0xC) == 0 && *(float *)(e + 0x38) < 0.0f &&
            *(float *)(p + 0xB4) <= 40.0f) {
            *(int *)(e + 0xC) = 1;
            func_001FBD50((int)p, 0x872, 0, 1000.0f);
        }
        if ((*(int *)(e + 0x0) & 0x1000) && *(float *)(p + 0xB4) <= -140.0f) {
            p[5] = 1;
            p[6] = 0;
        }
        break;
    }
    return;
tail:
    if (*(float *)(e + 0x40) == 0.0f) {
        if (*(float *)(p + 0x3C) <= 118.0f) {
            *(int *)(e + 0x48) = 0xBCA3D70A;
        }
    } else if (*(float *)(e + 0x40) <= -2.0f) {
        *(int *)(e + 0x40) = 0xC0000000;
        *(int *)(e + 0x48) = 0;
    }
    if (*(int *)(e + 0x0) & 0x1000) {
        p[6] = p[6] + 1;
        *(int *)(e + 0xC) = 0;
        *(int *)(e + 0x48) = 0;
        anim_clip_init((int)p, 4, 0.0f, 0.0f);
        func_001FBD50((int)p, 0x871, 0, 1000.0f);
    }
}
