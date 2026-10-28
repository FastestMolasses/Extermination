// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 90.89% NEARMISS).
// Fixes read from the original instructions:
// - Callee prototypes follow their definitions: func_001029C0 takes one matrix
//   pointer, func_00102BB0 (dst, src, angle), func_001026A0 (dst, m, v), all void.
// - arg1.x is updated with a compound subtract and arg1.z with a spelled-out
//   subtract (this decides the record/vector load order and the arg1 copy point).
// - The half-scale factor multiplies each quotient into its own local before
//   the two adds.
// - The negated record angle goes through the same float temporary that later
//   holds the half scale, which fixes the FP register colouring.

// Vector/physics update. arg1 is a float vec (xyz at +0,+4,+8); arg2 indexes a
// table of base pointers at D_00265890, arg3*0xC indexes a sub-record. Subtracts
// the record's x/z from arg1.x/z, stashes the vec + 1.0f into the uncached
// scratch matrix at 0x70003600, builds a rotation from the record via
// func_001029C0/func_00102BB0 (negated record.z angle) and applies it through
// func_001026A0, reads the rotated x/z back, then scales by tuning constants
// D_00810154/D_00810158/D_0081015C (with the 0.10666667 / 0.08533333 / 0.5
// factors) to produce the final arg1.x/z.
extern void func_001026A0(float *dst, float *m, float *v);
extern void func_001029C0(float *m);
extern void func_00102BB0(float *dst, float *src, float angle);
extern int D_00265890[];
extern float D_00810154;
extern float D_00810158;
extern float D_0081015C;
extern float D_70003400[];
extern float D_70003600[];

void func_002117D0(int arg0, float *arg1, int arg2, int arg3) {
    int base;
    int off;
    float t5;
    float t7;   /* first the negated record angle, then the half scale */
    float n0;
    float n2;

    base = D_00265890[arg2];
    off = arg3 * 0xC;
    arg1[0] -= *(float *)(base + off + 0);
    arg1[2] = arg1[2] - *(float *)(base + off + 4);
    *(float *)0x70003600 = arg1[0];
    *(float *)0x70003604 = arg1[1];
    *(float *)0x70003608 = arg1[2];
    *(int *)0x7000360C = 0x3F800000;
    func_001029C0(D_70003400);
    t7 = -*(float *)(off + base + 8);
    func_00102BB0(D_70003400, D_70003400, t7);
    func_001026A0(D_70003600, D_70003400, D_70003600);
    arg1[0] = *(float *)0x70003600;
    arg1[2] = *(float *)0x70003608;
    t7 = D_00810154 / 2.0f;
    t5 = 0.10666667f * D_00810154;
    n0 = t7 * -(D_0081015C / t5);
    n2 = t7 * (D_00810158 / t5);
    arg1[0] += n0;
    arg1[2] += n2;
    arg1[0] *= 0.10666667f * D_00810154;
    arg1[2] *= 0.08533333f * D_00810154;
}
