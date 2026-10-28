// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 90.77% NEARMISS).
// Fixes read from the original instructions:
// - the three trailing accumulates write their destinations through literal
//   addresses (0x008101F0/F4/F8), so each load waits for the previous store and
//   the original strictly sequential load/add/store order comes back (named
//   globals let mwcc batch the loads and defer the stores).
// - the +13.8 slew is a compound `+=` (operand order of the add).
//
// Per-frame camera/transform update.
// Advances an angle D_00810214 by -1.1693707 then normalizes it via
// func_001B1470; slews D_00810204 by +13.8; rebuilds a transform from
// D_70003400 (func_001029C0 / func_00102C58 with D_00810210) and seeds the
// scratchpad column 0x70003600 = {0,0,-20.0f,1.0f} (stored as raw int bit
// patterns, literal address per the func_001B15D0 idiom); transforms a point
// (func_001026A0 -> D_008101F0..) and accumulates D_008101F0/F4/F8 by
// D_00810200 / (D_00810204+D_0081023C) / D_00810208; finally func_0018D7B0.
// Returns 1. Same shape as func_001B0080's update path.
extern void func_00102948(float *, float *);
extern float func_001B1470(float);
extern void func_001029C0(float *);
extern void func_00102C58(float *, float *, float *);
extern void func_001026A0(float *, float *, int *);
extern int func_0018D7B0(float *, int);
extern float D_008101E0;
extern float D_008101F0;
extern float D_00810200;
extern float D_00810204;
extern float D_00810208;
extern float D_00810210;
extern float D_00810214;
extern float D_0081023C;
extern float D_00810350;
extern float D_70003400;
extern int D_70003600[4];
extern float D_70003B50;

int func_001BC560(void) {
    func_00102948(&D_00810210, &D_70003B50);
    D_00810214 = D_00810214 - 1.1693707f;
    D_00810214 = func_001B1470(D_00810214);
    func_00102948(&D_00810200, &D_00810350);
    D_00810204 += 13.8f;
    func_001029C0(&D_70003400);
    func_00102C58(&D_70003400, &D_70003400, &D_00810210);
    {
        int *sp = (int *)0x70003600;
        sp[0] = 0;
        sp[1] = 0;
        sp[2] = 0xC1A00000;
        sp[3] = 0x3F800000;
    }
    func_001026A0(&D_008101F0, &D_70003400, D_70003600);
    *(float *)0x008101F0 += D_00810200;
    *(float *)0x008101F4 += D_00810204 + D_0081023C;
    *(float *)0x008101F8 += D_00810208;
    func_0018D7B0(&D_008101E0, 1);
    return 1;
}
