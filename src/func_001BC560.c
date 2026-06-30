// NEARMISS func_001BC560  (vram 0x001BC560, 0x16C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 90.77% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// mwcc software-pipelines the three independent trailing global float accumulates (D_008101F0/F4/F8): it batches their loads/adds across the independent stores into many distinct FP temp regs and defers the stores, where CW emitted each load/add/store strictly sequentially reusing one reg pair. All...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// NEARMISS (90.8% mwcc 2.3.3 / 86.2% 991202): per-frame camera/transform update.
// Advances an angle D_00810214 by -1.1693707 then normalizes it via
// func_001B1470; slews D_00810204 by +13.8; rebuilds a transform from
// D_70003400 (func_001029C0 / func_00102C58 with D_00810210) and seeds the
// scratchpad column 0x70003600 = {0,0,-20.0f,1.0f} (stored as raw int bit
// patterns, literal address per the func_001B15D0 idiom); transforms a point
// (func_001026A0 -> D_008101F0..) and accumulates D_008101F0/F4/F8 by
// D_00810200 / (D_00810204+D_0081023C) / D_00810208; finally func_0018D7B0.
// Returns 1. All calls, the angle/slew math, and the scratchpad seed match
// 100%; sole residual is mwcc software-pipelining the three independent
// trailing global float accumulates (it batches their loads/adds and defers
// the stores) where CW emitted them strictly sequentially. Pure scheduling /
// register-coloring permutation -> permuter territory.
extern void func_00102948(float *, float *);
extern float func_001B1470(float);
extern void func_001029C0(float *);
extern void func_00102C58(float *, float *, float *);
extern void func_001026A0(float *, float *, int *);
extern int func_0018D7B0(float *, int);
extern float D_008101E0;
extern float D_008101F0;
extern float D_008101F4;
extern float D_008101F8;
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
    D_00810204 = D_00810204 + 13.8f;
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
    D_008101F0 += D_00810200;
    D_008101F4 += D_00810204 + D_0081023C;
    D_008101F8 += D_00810208;
    func_0018D7B0(&D_008101E0, 1);
    return 1;
}
