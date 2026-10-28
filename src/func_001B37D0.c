// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 81.85% NEARMISS).
// Fixes read from the original instructions:
// - The goto/parity-variable loop was really two plain nested for-loops
//   (7 rings, 2 signs each) with an early return on a failed probe.
// - The 4-word block handed to func_001026A0 is one local float array
//   {0, 3.0f, fparg0, 1.0f}, not four separate scalars.
// - func_001026A0/func_001028B8 take three pointer arguments (void *).
//
// Tries to place an entity (self) at a spread of angles around its facing
// (self+0xC4); w seeds the third word of the offset block, radius is the
// placement test radius. Builds a rotation via func_001B1470(PI + facing)
// into the D_70003400 matrix (func_001029C0/00102BB0), composes it with the
// block {0, 3.0f, w, 1.0f} and the entity origin (self+0xB0) into D_70003600
// (func_001026A0/001028B8), and probes with func_001B3250(self, mat, radius).
// If the straight-ahead probe fails, returns that angle. Otherwise sweeps up
// to 7 rings (i=1..7), each testing -off then +off where off = (PI/8)*i,
// using func_001B1470(PI + facing -/+ off); returns the first angle whose
// probe fails (returns 0), else the last angle tried.
extern float func_001B1470(float);
extern void func_001029C0(void *);
extern void func_00102BB0(void *, void *, float);
extern void func_001026A0(void *, void *, void *);
extern void func_001028B8(void *, void *, void *);
extern int func_001B3250(char *, void *, float);
extern int D_70003400;
extern int D_70003600;

float func_001B37D0(char *self, float w, float radius) {
    float blk[4];
    float ang;
    float off;
    int i;
    int j;

    ang = func_001B1470(3.1415927f + *(float *)(self + 0xC4));
    func_001029C0(&D_70003400);
    func_00102BB0(&D_70003400, &D_70003400, ang);
    blk[0] = 0.0f;
    blk[1] = 3.0f;
    blk[2] = w;
    blk[3] = 1.0f;
    func_001026A0(&D_70003600, &D_70003400, blk);
    func_001028B8(&D_70003600, &D_70003600, self + 0xB0);
    if (func_001B3250(self, &D_70003600, radius) == 0) {
        return ang;
    }
    for (i = 0; i < 7; i++) {
        off = 0.3926991f * (float)(i + 1);
        for (j = 0; j < 2; j++) {
            if (j & 1) {
                ang = func_001B1470(3.1415927f + *(float *)(self + 0xC4) + off);
            } else {
                ang = func_001B1470((3.1415927f + *(float *)(self + 0xC4)) - off);
            }
            func_001029C0(&D_70003400);
            func_00102BB0(&D_70003400, &D_70003400, ang);
            func_001026A0(&D_70003600, &D_70003400, blk);
            func_001028B8(&D_70003600, &D_70003600, self + 0xB0);
            if (func_001B3250(self, &D_70003600, radius) == 0) {
                return ang;
            }
        }
    }
    return ang;
}
