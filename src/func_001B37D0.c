// NEARMISS func_001B37D0  (vram 0x001B37D0, 0x218 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 81.85% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Identical call set, arguments, and nested (7 rings x 2 signs) loop structure as the target, including the beql/bnel branch-likely on func_001B3250. Residual is fp-register coloring plus entry-sequence scheduling: target promotes fparg0->$f22 and fparg1->$f20 at function entry and orders the arg-s...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// NEARMISS 81.9% (mwcc 2.3.3; pinned 991202 build 75.6%). Logic fully recovered.
//
// Tries to place an entity (self=arg0) at a spread of angles around its facing
// (arg0[0xC4]); fparg0 seeds the transform's +8 word, fparg1 is the placement
// test radius. Builds a rotation via func_001B1470(PI + facing) into the
// D_70003400 matrix (func_001029C0/00102BB0), composes it with a 4-word block
// {0, 3.0f, fparg0, 1.0f} and the entity origin (arg0+0xB0) into D_70003600
// (func_001026A0/001028B8), and probes with func_001B3250(self, mat, fparg1).
// If the straight-ahead probe fails, returns that angle. Otherwise sweeps up to
// 7 rings (i=1..7), each testing +off then -off where off = (PI/8)*i, using
// func_001B1470(PI + facing +/- off); returns the first angle whose probe
// succeeds (func_001B3250 != 0 continues, ==0 returns).
//
// WALL: same calls, args, and nested-loop structure as the target; the residual
// is fp-register coloring + instruction scheduling of the entry/per-iteration
// call chain -- the target promotes fparg0->$f22 and fparg1->$f20 at entry and
// orders the arg-save mov.s / add.s / sp50-block stores (swc1 $f22,sp+8)
// differently from mwcc's schedule. FP-coloring + scheduling-permutation class.
extern float func_001B1470(float);
extern void func_001029C0(void *);
extern void func_00102BB0(void *, void *, float);
extern void func_001026A0(void *, void *, int *);
extern void func_001028B8(void *, void *, char *);
extern int func_001B3250(char *, void *, float);
extern int D_70003400;
extern int D_70003600;

float func_001B37D0(char *arg0, float fparg0, float fparg1) {
    int sp50;
    int sp54;
    float sp58;
    int sp5C;
    float ang;
    float off;
    int i;
    int j;
    int s0;
    int v0_2;

    ang = func_001B1470(3.1415927f + *(float *)(arg0 + 0xC4));
    func_001029C0(&D_70003400);
    func_00102BB0(&D_70003400, &D_70003400, ang);
    sp50 = 0;
    sp54 = 0x40400000;
    sp58 = fparg0;
    sp5C = 0x3F800000;
    func_001026A0(&D_70003600, &D_70003400, &sp50);
    func_001028B8(&D_70003600, &D_70003600, arg0 + 0xB0);
    if (func_001B3250(arg0, &D_70003600, fparg1) == 0) {
        return ang;
    }
    i = 1;
    s0 = 0;
loop_4:
    j = 0;
    off = 0.3926991f * (float) i;
    v0_2 = 0 & 1;
loop_5:
    if (v0_2 != 0) {
        ang = func_001B1470(3.1415927f + *(float *)(arg0 + 0xC4) + off);
    } else {
        ang = func_001B1470((3.1415927f + *(float *)(arg0 + 0xC4)) - off);
    }
    func_001029C0(&D_70003400);
    func_00102BB0(&D_70003400, &D_70003400, ang);
    func_001026A0(&D_70003600, &D_70003400, &sp50);
    func_001028B8(&D_70003600, &D_70003600, arg0 + 0xB0);
    if (func_001B3250(arg0, &D_70003600, fparg1) != 0) {
        j += 1;
        v0_2 = j & 1;
        if (j < 2) {
            goto loop_5;
        }
        s0 += 1;
        i = s0 + 1;
        if (s0 < 7) {
            goto loop_4;
        }
    }
    return ang;
}
