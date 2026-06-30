// NEARMISS func_0012CAA0  (vram 0x0012CAA0, 0x7A0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.50% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Permuter-class register/scheduling artifacts on an otherwise byte-faithful body (3 residual clusters): (1) commutative addu operand canonicalization 'addu v0,v0,s0' vs target 'addu v0,s0,v0' at the D_00242DD0[s1] bin-pointer compute (x3 sites); (2) two-immediate-const FP arg ordering for the 3rd ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Per-enemy boarding/mount animation state-machine, dispatched on the state byte at arg0[6] (3 states 0/1/2,
// case 0 falls through into case 1). arg0 = the enemy/actor, arg1 = the boarding-target block; s1 = (arg1->0xF6 & 7)
// selects one of 8 0x18-byte mount-offset records in the parallel float tables D_00242DF0/DF4/DF8/DFC/E00/E04, and
// D_00242DD0[s1] indexes into the player struct D_008102B0 (its +0x110 -> bone matrix, +0x90). The matrix helpers
// (func_001029C0 identity, func_00102B08/BB0/A60 axis rotates, func_001026D0 transform, func_001031E0 copy,
// func_001026A0 transform-point, copy_qw4) build the world transform; func_001B12B0 steps the heading toward the
// player, func_001B1470 wraps angles, func_0011E2A8/DE90 are sin/cos. State 0 seeds the offsets, anchors arg1+0x10
// from the player bone, sets the 1.0f timer at arg1+0xD4. State 1 eases heading/position each frame, decrements the
// 0xD4 timer, and on <=0.2 advances the state, re-seeds offsets and kicks anim 0x16 (func_001287F0) with a 0xF0-frame
// timer at arg1+0xD0. State 2 holds the mount transform, and once (arg0[7]==0 && arg1->0xF4 & 0x4000) fires the
// mounted shot: picks a speed const into D_008102B0+0x224 (by arg1->0xE1 and global D_0081070A), builds the
// projectile pos/dir at 0x700038A0/B0, stores sin/cos at D_008102B0+0x70/+0x78, spawns via func_001EFD90(0x80000006)
// and func_001FBD50(self,0x1B2,0,300.0f); then counts down the 0xD0 timer and on 0 returns the enemy to state 7.
// Tail: a global stun check (D_008106BC / *0x70003B8D) forces state 0xA.
//
// WALL: permuter-class register-coloring + scheduling on an otherwise byte-faithful body. Residuals: commutative
// addu operand order at the D_00242DD0[s1] bin-pointer compute (addu v0,v0,s0 vs v0,s0,v0, x3); two-immediate FP arg
// ordering on func_001B12B0(-1.5707964f,...,0.06981317f) (target loads 0.06981317->f14 before -1.5707964->f12; the
// assign-in-arg idiom cannot flip two lui/ori immediates); and zero-store vs 0x3F800000-store scheduling in the
// s1>=4 matrix-setup branch. Not the clean-store nop, not idiom-fixable -> parked.
extern void func_001029C0(void *a0);
extern void func_00102B08(void *a0, void *a1, float f);
extern void func_00102BB0(void *a0, void *a1, float f);
extern void func_00102A60(void *a0, void *a1, float f);
extern void func_001026A0(void *a0, void *a1, void *a2);
extern void func_001026D0(void *a0, void *a1, void *a2);
extern void func_001031E0(void *a0, void *a1);
extern void func_00103230(void *a0, void *a1, float f);
extern void copy_qw4(void *a0, void *a1);
extern float func_001B12B0(float a, float b, float c);
extern float func_001B1470(float a);
extern float func_0011E2A8(float a);
extern float func_0011DE90(float a);
extern void func_001287F0(int a0, int a1, int a2, float f0);
extern void func_001EFD90(int a0, void *a1, void *a2);
extern int func_001FBD50(void *a0, int a1, int a2, float f12);
extern int D_00242DD0[];
extern char D_00242DF0[];
extern char D_00242DF4[];
extern char D_00242DF8[];
extern char D_00242DFC[];
extern char D_00242E00[];
extern char D_00242E04[];
extern char D_008102B0[];
extern char D_70003000[];
extern char D_70003030[];
extern char D_700036A0[];
extern char D_700036D0[];
extern char D_700038A0[];
extern char D_700038B0[];
extern unsigned char D_0081070A;
extern unsigned char D_008106BC;

void func_0012CAA0(char *arg0, char *arg1) {
    short a1v = *(short *)(arg1 + 0xF6);
    char *plr = D_008102B0;
    int s1 = a1v & 7;

    switch (*(unsigned char *)(arg0 + 6)) {
    case 0:
        *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
        *(float *)(arg1 + 0x30) = *(float *)(D_00242DF0 + s1 * 0x18);
        *(float *)(arg1 + 0x34) = *(float *)(D_00242DF4 + s1 * 0x18);
        *(float *)(arg1 + 0x38) = *(float *)(D_00242DF8 + s1 * 0x18);
        *(float *)(arg1 + 0x40) = *(float *)(D_00242DFC + s1 * 0x18);
        *(float *)(arg1 + 0x44) = *(float *)(D_00242E00 + s1 * 0x18);
        *(float *)(arg1 + 0x48) = *(float *)(D_00242E04 + s1 * 0x18);
        func_001029C0(D_700036A0);
        func_00102B08(D_700036A0, D_700036A0, *(float *)(arg1 + 0x40));
        func_00102BB0(D_700036A0, D_700036A0, *(float *)(arg1 + 0x44));
        func_00102A60(D_700036A0, D_700036A0, *(float *)(arg1 + 0x48));
        func_001031E0(D_700036D0, arg1 + 0x30);
        func_001026D0(D_700036A0, *(char **)(D_00242DD0[s1] * 4 + (int)plr + 0x110) + 0x90, D_700036A0);
        *(float *)(arg1 + 0x10) = *(float *)0x700036D0 - *(float *)(arg0 + 0xB0);
        *(float *)(arg1 + 0x14) = *(float *)0x700036D4 - *(float *)(arg0 + 0xB4);
        *(float *)(arg1 + 0x18) = *(float *)0x700036D8 - *(float *)(arg0 + 0xB8);
        *(int *)(arg1 + 0x1C) = 0x3F800000;
        *(float *)(arg1 + 0xE8) = *(float *)(plr + 0xC4);
        *(float *)(arg1 + 0xD4) = 1.0f;
        /* fallthrough */
    case 1:
        if (s1 >= 4) {
            *(float *)(arg0 + 0xC4) = func_001B12B0(*(float *)(plr + 0xC4), *(float *)(arg0 + 0xC4), 0.06981317f);
        } else {
            *(float *)(arg0 + 0xC4) = func_001B12B0(3.1415927f + *(float *)(plr + 0xC4), *(float *)(arg0 + 0xC4), 0.06981317f);
        }
        *(float *)(arg0 + 0xC0) = func_001B12B0(-1.5707964f, *(float *)(arg0 + 0xC0), 0.06981317f);
        {
            float d = *(float *)(arg1 + 0xD4) - 0.02f;
            *(float *)(arg1 + 0xD4) = d;
            func_00103230(arg1 + 0x10, arg1 + 0x10, d);
        }
        {
            float ang = func_001B1470(*(float *)(plr + 0xC4) - *(float *)(arg1 + 0xE8));
            func_001029C0(D_700036A0);
            func_00102BB0(D_700036A0, D_700036A0, func_001B1470(3.1415927f + ang));
        }
        func_001026A0(D_700038A0, D_700036A0, arg1 + 0x10);
        func_001029C0(D_700036A0);
        func_00102B08(D_700036A0, D_700036A0, *(float *)(arg1 + 0x40));
        func_00102BB0(D_700036A0, D_700036A0, *(float *)(arg1 + 0x44));
        func_00102A60(D_700036A0, D_700036A0, *(float *)(arg1 + 0x48));
        func_001031E0(D_700036D0, arg1 + 0x30);
        func_001026D0(D_700036A0, *(char **)(D_00242DD0[s1] * 4 + (int)plr + 0x110) + 0x90, D_700036A0);
        *(float *)(arg0 + 0xB0) = *(float *)0x700036D0 + *(float *)0x700038A0;
        *(float *)(arg0 + 0xB4) = *(float *)0x700036D4 - *(float *)0x700038A4;
        *(float *)(arg0 + 0xB8) = *(float *)0x700036D8 + *(float *)0x700038A8;
        func_001029C0(D_70003000);
        func_00102B08(D_70003000, D_70003000, *(float *)(arg0 + 0xC0));
        func_00102BB0(D_70003000, D_70003000, *(float *)(arg0 + 0xC4));
        func_00102A60(D_70003000, D_70003000, *(float *)(arg0 + 0xC8));
        copy_qw4(arg0 + 0xD0, D_70003000);
        func_001031E0(D_70003030, arg0 + 0xB0);
        if (*(float *)(arg1 + 0xD4) <= 0.2f) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
            *(unsigned char *)(arg0 + 7) = 0;
            *(float *)(arg1 + 0x30) = *(float *)(D_00242DF0 + s1 * 0x18);
            *(float *)(arg1 + 0x34) = *(float *)(D_00242DF4 + s1 * 0x18);
            *(float *)(arg1 + 0x38) = *(float *)(D_00242DF8 + s1 * 0x18);
            *(float *)(arg0 + 0xC0) = *(float *)(arg1 + 0x40);
            *(float *)(arg0 + 0xC4) = *(float *)(arg1 + 0x44);
            *(float *)(arg0 + 0xC8) = *(float *)(arg1 + 0x48);
            func_001287F0((int)arg0, (int)arg1, 0x16, 0.0f);
            *(short *)(arg1 + 0xD0) = 0xF0;
        }
        break;
    case 2:
        func_001029C0(D_70003000);
        func_00102B08(D_70003000, D_70003000, *(float *)(arg0 + 0xC0));
        func_00102BB0(D_70003000, D_70003000, *(float *)(arg0 + 0xC4));
        func_00102A60(D_70003000, D_70003000, *(float *)(arg0 + 0xC8));
        func_001031E0(D_70003030, arg1 + 0x30);
        func_001026D0(D_70003000, *(char **)(D_00242DD0[s1] * 4 + (int)plr + 0x110) + 0x90, D_70003000);
        func_001031E0(arg0 + 0xB0, D_70003030);
        if (*(unsigned char *)(arg0 + 7) == 0 && (*(short *)(arg1 + 0xF4) & 0x4000)) {
            float f20;
            if (*(unsigned char *)(arg1 + 0xE1) == 0) {
                if (D_0081070A == 0) {
                    *(int *)(plr + 0x224) = 0x41200000;
                } else {
                    *(int *)(plr + 0x224) = 0x41700000;
                }
            } else {
                if (D_0081070A == 0) {
                    *(int *)(plr + 0x224) = 0x41900000;
                } else {
                    *(int *)(plr + 0x224) = 0x41B00000;
                }
            }
            *(float *)0x700038A0 = 0.0f;
            *(float *)0x700038A4 = 1.0f;
            *(float *)0x700038A8 = 3.0f;
            *(int *)0x700038AC = 0x3F800000;
            func_001026A0(D_700038A0, D_70003000, D_700038A0);
            if (s1 >= 4) {
                f20 = func_001B1470(3.1415927f + *(float *)(plr + 0xC4));
                *(int *)0x700038B0 = 0;
                *(float *)0x700038B4 = f20;
                *(int *)0x700038B8 = 0;
            } else {
                f20 = *(float *)(plr + 0xC4);
                *(int *)0x700038B0 = 0;
                *(float *)0x700038B4 = f20;
                *(int *)0x700038B8 = 0;
            }
            *(int *)0x700038BC = 0x3F800000;
            *(float *)(plr + 0x70) = func_0011E2A8(f20);
            *(int *)(plr + 0x74) = 0;
            *(float *)(plr + 0x78) = func_0011DE90(f20);
            *(int *)(plr + 0x7C) = 0x3F800000;
            func_001EFD90(0x80000006, D_700038A0, D_700038B0);
            *(unsigned char *)(arg0 + 7) = 1;
            func_001FBD50(arg0, 0x1B2, 0, 300.0f);
        }
        {
            short t = *(short *)(arg1 + 0xD0) - 1;
            *(short *)(arg1 + 0xD0) = t;
            if (t == 0) {
                *(char *)(arg0 + 5) = 7;
                *(unsigned char *)(arg0 + 6) = 0;
                *(unsigned char *)(arg0 + 7) = 0;
            }
        }
        break;
    }
    if (D_008106BC == 0 && *(unsigned char *)0x70003B8D == 0) {
        return;
    }
    *(char *)(arg0 + 5) = 0xA;
    *(unsigned char *)(arg0 + 6) = 0;
    *(unsigned char *)(arg0 + 7) = 0;
}
