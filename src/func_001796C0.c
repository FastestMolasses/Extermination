// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// TU_STATIC: func_00179680
//
// Byte-matched (objdiff 100%) with func_00179680 compiled as a static
// function of this file. The original compiler saw that callee earlier in the
// same translation unit, knew its register use and built the -4.01f constant
// in a0, where a one-function file gets v0 (the former "regalloc colouring" NEARMISS residual,
// 2 of 112 instructions). tools/decomp/tu_static.py removes the static copy
// after checking it equals the original func_00179680; the call links to that
// function's own unit.

//
// Per-frame update for an actor's "wind-down / recoil" timer.
//
// Bails immediately if the +0x25F lock byte is set. Picks a rate scale from the
// +0x1F0 state byte (state 0x3A -> 2.0f, otherwise 3.0f).
//
// If the +0xA flag byte is set, the accumulator at +0x2EC is zeroed and the
// actor is pushed into state 0x1C / substate 0x30 (clearing +0x6) -- but only
// if the +0x5 state is neither 0x1D nor 0x1E and the +0x237 event byte is set.
//
// Otherwise the accumulator at +0x2EC counts down by -0.04f per frame while it
// is still above -0.04f * rate, adding each step into the +0xB4 angle.
//
// Once the accumulator crosses that limit, func_00179450() is asked whether a
// trigger entry was passed. If not, func_00179680() resets the actor. If it was
// and the reported delta at +0x258 is below -4.01f, func_00179680() also resets.
// Otherwise the accumulator continues to decay, clamped at -4.0f, is folded into
// the +0xB4 angle, and func_0019AB20() is run over the +0xB0 / +0x280 vectors;
// if that succeeds and the global object at *0x700031D0 has (halfword +0x1A &
// 0xFF00) == 0x1000, func_00179680() resets the actor.
extern int func_00179450(unsigned char *p, float *v);
static void func_00179680(unsigned char *p);  /* same-TU callee, see TU_STATIC */
#include "src/func_00179680.c"
extern int func_0019AB20(unsigned char *p, float *a, float *b, int n);

void func_001796C0(unsigned char *p) {
    float rate;

    if (p[0x25F] != 0) {
        return;
    }
    if (p[0x1F0] == 0x3A) {
        rate = 2.0f;
    } else {
        rate = 3.0f;
    }
    if (p[0xA] != 0) {
        *(float *)(p + 0x2EC) = 0.0f;
        if (p[5] == 0x1D) {
            return;
        }
        if (p[5] == 0x1E) {
            return;
        }
        if (p[0x237] == 0) {
            return;
        }
        p[5] = 0x1C;
        p[6] = 0;
        p[0x1F0] = 0x30;
        return;
    }
    if (*(float *)(p + 0x2EC) > -0.04f * rate) {
        *(float *)(p + 0x2EC) += -0.04f;
        *(float *)(p + 0xB4) += *(float *)(p + 0x2EC);
        return;
    }
    if (func_00179450(p, (float *)(p + 0xB0)) != 0) {
        if (*(float *)(p + 0x258) < -4.01f) {
            func_00179680(p);
            return;
        }
        *(float *)(p + 0x2EC) += -0.04f;
        if (*(float *)(p + 0x2EC) < -4.0f) {
            *(float *)(p + 0x2EC) = -4.0f;
        }
        *(float *)(p + 0xB4) += *(float *)(p + 0x2EC);
        if (func_0019AB20(p, (float *)(p + 0xB0), (float *)(p + 0x280), 6) != 0) {
            if ((*(short *)(*(int *)0x700031D0 + 0x1A) & 0xFF00) == 0x1000) {
                func_00179680(p);
            }
        }
    } else {
        func_00179680(p);
    }
}
