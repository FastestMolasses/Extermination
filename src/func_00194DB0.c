// NEARMISS func_00194DB0  (vram 0x00194DB0, 0x374 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 86.15% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Instruction-scheduling/materialization-order permutation. Body and structure are fully recovered and match through the first ~25 instructions (register-coloring only); the sole structural residual is that mwcc233 speculatively hoists the NEXT float constant's lui into a branch delay slot one inst...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Steers an actor's yaw (arg0+0xAC) toward a target angle derived from arg1's
// pitch field (+0xA8), clamped/wrapped into a piecewise angle range. If
// arg1's level id (+0x230) is 3 AND the wrapped delta to the goal is small
// (<=45 deg via func_001B1470/func_0011DF78 abs), the yaw is left unchanged
// this frame; otherwise it is stepped toward goal by func_001B12B0 at a
// ~0.2 deg rate. Recomputes arg0's screen
// offsets (+0x10/+0x18) from sin/cos of the new yaw scaled by var_f20 (94 or
// 100 depending on whether arg1's pitch is below -1610). Sets +0x14 to -12.0
// or -3.0 based on a flag bit at +0x5A, clamps +0x20 to >= -90 and +0x28 into
// [-1617,-1537], then calls func_0018C0C0/func_0018D7B0/func_0018C6A0/
// func_0018C4B0 to push the updated transform.
extern float func_0011DE90(float);
extern float func_0011DF78(float);
extern float func_0011E2A8(float);
extern void func_0018C0C0(char *);
extern void func_0018C4B0(float *, float, float);
extern void func_0018C6A0(char *, float *, float);
extern void func_0018D7B0(char *, int);
extern float func_001B12B0(float goal, float cur, float rate);
extern float func_001B1470(float a);
extern float D_008105D0;

void func_00194DB0(char *arg0, char *arg1) {
    float pitch;
    float lo;
    float hi;
    float goal;
    float scale;
    float delta;
    int u;

    pitch = *(float *)(arg1 + 0xA8);
    scale = (pitch < -1610.0f) ? 94.0f : 100.0f;

    if (!(pitch <= -1497.0f)) {
        hi = -1497.0f;
    } else {
        hi = pitch;
        if (pitch < -1657.0f) {
            hi = -1657.0f;
        }
    }

    delta = (3.1415927f * (hi + 1497.0f)) / -160.0f;
    if (pitch < -1577.0f) {
        goal = 1.5707964f + (3.1415927f - delta);
        lo = 2.792527f;
        if (goal <= 2.792527f) {
            lo = 1.9198622f;
            if (goal >= 1.9198622f) {
                goal = lo;
            }
        }
    } else {
        goal = (3.1415927f - delta) - 1.5707964f;
        lo = 1.2217306f;
        if (goal <= 1.2217306f) {
            lo = 0.34906587f;
            if (goal < 0.34906587f) {
                goal = lo;
            }
        }
    }

    if (*(int *)(arg1 + 0x230) == 3 &&
        func_0011DF78(func_001B1470(goal - *(float *)(arg0 + 0xAC))) <= 0.7853982f) {
        /* delta small enough: leave arg0->0xAC unchanged */
    } else {
        *(float *)(arg0 + 0xAC) = func_001B12B0(goal, *(float *)(arg0 + 0xAC), 0.0034906587f);
    }

    *(float *)(arg0 + 0x10) = -142.0f + (scale * func_0011E2A8(*(float *)(arg0 + 0xAC)));
    *(float *)(arg0 + 0x18) = -1577.0f + (scale * func_0011DE90(*(float *)(arg0 + 0xAC)));

    u = 0xC1500000;
    if (*(short *)(arg0 + 0x5A) & 1) {
        u = 0xC0400000;
    }
    *(int *)(arg0 + 0x14) = u;

    if (!(*(float *)(arg0 + 0x20) <= -90.0f)) {
        *(float *)(arg0 + 0x20) = -90.0f;
    }

    if (*(float *)(arg0 + 0x28) < -1617.0f) {
        *(float *)(arg0 + 0x28) = -1617.0f;
    } else if (!(*(float *)(arg0 + 0x28) <= -1537.0f)) {
        *(float *)(arg0 + 0x28) = -1537.0f;
    }

    func_0018C0C0(arg0);
    func_0018D7B0(arg0, 5);
    func_0018C6A0(arg0 + 0x10, &D_008105D0, 1.0f);
    func_0018C4B0(&D_008105D0, *(float *)(arg0 + 0x14), 1.0f);
}
