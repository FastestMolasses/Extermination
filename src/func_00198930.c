// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Builds a muzzle/effect transform off entity arg1 and writes it into arg0,
// then OR-folds four spawn results into a packed return code. Resets arg0+0x30
// via func_00102948(&D_70003B50); loads an identity-ish matrix at D_70003400
// (func_001029C0) and concatenates it with arg0+0x30 (func_00102C58). Computes
// a local-space offset position into arg0+0x10/0x14/0x18 from arg1's heading:
//   arg0[0x10] = arg1[0xA0] + -6.0f*sin(arg1[0xC4])
//   arg0[0x14] = 6.0f + arg1[0xB4]
//   arg0[0x18] = arg1[0xA8] + -6.0f*cos(arg1[0xC4])
// Spawns two effects (func_0018C4B0/func_0018C6A0 against D_008105D0, bias
// 0.02f) and ORs their results into the accumulator r. Builds the scratchpad
// vector at 0x70003600 = {0,0,20.0f,0}, transforms it through D_70003400 into
// arg0+0x20 (func_001026A0), adds the offset (arg0+0x10..) into arg0+0x20..,
// then spawns two more effects (D_008105E0) and folds them in: the return is
// (((c4b0|c6a0) << 4) | c4b0 | c6a0).
//
// Matched 100.0 with mwcc 2.3.3 (mwcps2-2.3.3-000906); the pinned 991202 build
// reaches 98.2%. Keys: r is a callee-saved OR-accumulator init to 0 before the
// first call (fills the func_00102948 delay slot); the `r <<= 4` is hoisted to
// its own statement before the func_001026A0 block so it stays in s0 across the
// calls; the three position adds are written `dest += addend` (not
// `dest = addend + dest`) to land the target's fv0/fv0f FP pairing; and the
// scratchpad vector is built via literal stores while &D_70003600 is passed as
// the symbol arg. Verified objdiff 100.0 vs build/expected/func_00198930.o.

extern void func_00102948(void *dst, void *src);
extern void func_001029C0(float *m);
extern void func_00102C58(float *dst, float *src, void *v);
extern void func_001026A0(void *dst, float *m, void *v);
extern float func_0011E2A8(float a); /* sin */
extern float func_0011DE90(float a); /* cos */
extern int func_0018C4B0(void *p, float a, float b);
extern int func_0018C6A0(void *p, void *q, float b);
extern float D_70003B50[];
extern float D_70003400[];
extern float D_008105D0[];
extern float D_008105E0[];
extern float D_70003600[4];

int func_00198930(char *arg0, char *arg1) {
    int r;

    r = 0;
    func_00102948(arg0 + 0x30, D_70003B50);
    func_001029C0(D_70003400);
    func_00102C58(D_70003400, D_70003400, arg0 + 0x30);

    *(float *)(arg0 + 0x10) = *(float *)(arg1 + 0xA0) + -6.0f * func_0011E2A8(*(float *)(arg1 + 0xC4));
    *(float *)(arg0 + 0x14) = 6.0f + *(float *)(arg1 + 0xB4);
    *(float *)(arg0 + 0x18) = *(float *)(arg1 + 0xA8) + -6.0f * func_0011DE90(*(float *)(arg1 + 0xC4));

    r |= func_0018C4B0(D_008105D0, *(float *)(arg0 + 0x14), 0.02f);
    r |= func_0018C6A0(arg0 + 0x10, D_008105D0, 0.02f);
    r <<= 4;

    *(int *)0x70003600 = 0;
    *(int *)0x70003604 = 0;
    *(float *)0x70003608 = 20.0f;
    *(int *)0x7000360C = 0;
    func_001026A0(arg0 + 0x20, D_70003400, D_70003600);

    *(float *)(arg0 + 0x20) += *(float *)(arg0 + 0x10);
    *(float *)(arg0 + 0x24) += *(float *)(arg0 + 0x14);
    *(float *)(arg0 + 0x28) += *(float *)(arg0 + 0x18);

    r |= func_0018C4B0(D_008105E0, *(float *)(arg0 + 0x24), 0.02f);
    r |= func_0018C6A0(arg0 + 0x20, D_008105E0, 0.02f);
    return r;
}
