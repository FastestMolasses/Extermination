// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 94.69% NEARMISS).
// Fixes read from the original instructions:
//  - The rate table is a struct copy, not a 128-bit integer store through a cast
//    pointer: copying a struct of four floats makes mwcc form the full source
//    address in a register before the quadword load (the "reloc-pair interleave"
//    was never a compiler wall), which also restores the FP register colouring.
//
// SEMANTICS (FINDINGS "KNIFE/MELEE DECODED"): heavy-stab in-swing yaw steer
// (knife SQUARE attack, mode 0x22 states 2/3): while the stab swings, steer the
// player yaw +0xC4 toward the goal heading +0x218 at pi * D_002486F0[gait] / 180
// rad/tick (D_002486F0 = {0,0.5,1.0,2.0}f rate table indexed by gait byte
// +0x23F; the table is copied to the stack first), gated on
// func_00174AC0(self,2); the steer helper is func_001B12B0(goal,cur,rate)->yaw.
typedef struct { float f[4]; } Rates;
extern Rates D_002486F0;
extern int func_00174AC0(char *p, int n);
extern float func_001B12B0(float goal, float cur, float rate);

void func_00173DD0(unsigned char *self) {
    Rates rates = D_002486F0;
    if (func_00174AC0((char *)self, 2)) {
        *(float *)(self + 0xC4) = func_001B12B0(
            *(float *)(self + 0x218), *(float *)(self + 0xC4),
            (3.1415927410125732f * rates.f[self[0x23F]]) / 180.0f);
    }
}
