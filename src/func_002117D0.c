// NEARMISS func_002117D0  (vram 0x002117D0, 0x1A0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 90.89% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation permutation (FP odd-half coloring + arg-pointer lifetime split). Full body recovered: correct 4-arg signature (a0 unused, arg1=float*, arg2/arg3 indices), 0x40 frame with s0/s1/s2 saves, off+base addressing reused for the second access, and the two D_0081015C/t5 + D_00810158/t...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Vector/physics update. arg1 is a float vec (xyz at +0,+4,+8); arg2 indexes a
// table of base pointers at D_00265890, arg3*0xC indexes a sub-record. Subtracts
// the record's x/z from arg1.x/z, stashes the vec + 1.0f into the uncached
// scratch matrix at 0x70003600, builds a rotation from the record via
// func_001029C0/func_00102BB0 (negated record.z angle) and applies it through
// func_001026A0, reads the rotated x/z back, then scales by tuning constants
// D_00810154/D_00810158/D_0081015C (with the 0.10666667 / 0.08533333 / 0.5
// factors) to produce the final arg1.x/z.
//
// NEARMISS 90.89% via mwcc 2.3.3 (991202 reaches 84.55%). Body/frame/call-order
// faithful. Residual is register coloring: an a1<->s0 lifetime split on the arg1
// pointer in the first block + FP odd-half (fv0/fv0f) coloring in the scale
// chain. Register-allocation-order permuter territory.
extern int func_001026A0(void *a, void *b, void *c);
extern int func_001029C0(void *a, void *b);
extern int func_00102BB0(void *a, void *b, float f);
extern int D_00265890[];
extern float D_00810154;
extern float D_00810158;
extern float D_0081015C;
extern float D_70003400;
extern float D_70003600;

void func_002117D0(int arg0, float *arg1, int arg2, int arg3) {
    int base;
    int off;
    float t7;
    float t5;
    float n0;
    float n2;

    base = D_00265890[arg2];
    off = arg3 * 0xC;
    arg1[0] = arg1[0] - *(float *)(base + off + 0);
    arg1[2] = arg1[2] - *(float *)(base + off + 4);
    *(float *)0x70003600 = arg1[0];
    *(float *)0x70003604 = arg1[1];
    *(float *)0x70003608 = arg1[2];
    *(int *)0x7000360C = 0x3F800000;
    func_001029C0(&D_70003400, (void *)(base + off));
    func_00102BB0(&D_70003400, &D_70003400, -*(float *)(off + base + 8));
    func_001026A0(&D_70003600, &D_70003400, &D_70003600);
    arg1[0] = *(float *)0x70003600;
    arg1[2] = *(float *)0x70003608;
    t7 = D_00810154 / 2.0f;
    t5 = 0.10666667f * D_00810154;
    n0 = D_0081015C / t5;
    n2 = D_00810158 / t5;
    arg1[0] = arg1[0] + t7 * -n0;
    arg1[2] = arg1[2] + t7 * n2;
    arg1[0] = arg1[0] * (0.10666667f * D_00810154);
    arg1[2] = arg1[2] * (0.08533333f * D_00810154);
}
