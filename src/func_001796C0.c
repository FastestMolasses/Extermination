// NEARMISS func_001796C0  (vram 0x001796C0, 0x1B8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.91% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// REGALLOC COLORING of one short-lived scratch GPR — 2 of 112 instructions differ (99.909%). Sole residual, at the -4.01f test: target emits `lui v1,0xc080; ori a0,v1,0x51ec; mtc1 a0,$f0`, mwcc 2.3.3 emits `lui v1,0xc080; ori v0,v1,0x51ec; mtc1 v0,$f0`. mwcc picks v0 (lowest free temp) where CodeWa...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

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
extern void func_00179680(unsigned char *p);
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
