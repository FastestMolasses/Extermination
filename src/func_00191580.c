// NEARMISS func_00191580  (vram 0x00191580, 0x13C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 96.38% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FP-register-coloring permutation: body and control flow byte-structure identical; target uses the f1/f3 FP-temp pair where mwcc colors f4..f6 (consistent throughout). Not the clean-store delay-slot nop, so 2.3.3 does not fix it. Permuter (FP-coloring) territory.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// NEARMISS 96.9% (mwcc 991202) / 96.4% (mwcc 2.3.3). Body + control flow
// fully recovered and structurally identical; sole residual is FP-register
// coloring (target keeps f1/f3 pair where mwcc allocates f4..f6) on the
// camera/offset math. Not the clean-store nop -> 2.3.3 does not help.
// Permuter territory (FP-coloring permutation).
//
// Semantics: writes a smoothed pitch delta to scratchpad 0x70003A20
// (= D_0081069C - func_0011DF78(arg0+0xC)); if below a per-state floor
// (-20 when arg0+0x64 == -46.8f else -10) it damps the excess by 0.5 and
// clamps the stored value at 0x70003A24 to >= -1.5, then calls
// func_0018C5A0(arg0+0x10, 11.0 + composed_height, 4.0).

extern float func_0011DF78(float);
extern void func_0018C5A0(unsigned char *, float, float);
extern void func_001916C0(unsigned char *, unsigned char *, int);
extern float D_0081069C;

void func_00191580(unsigned char *arg0, unsigned char *arg1) {
    float t;
    float lo;
    float d;
    float v;

    func_001916C0(arg0, arg1, 0);
    *(float *)0x70003A20 = D_0081069C - func_0011DF78(*(float *)(arg0 + 0xC));
    if (*(float *)(arg0 + 0x64) == -46.8f) {
        lo = -20.0f;
    } else {
        lo = -10.0f;
    }
    t = *(float *)0x70003A20;
    if (t < lo) {
        d = 0.5f * (t - lo);
        *(float *)0x70003A24 = d;
        if (d < -1.5f) {
            *(float *)0x70003A24 = -1.5f;
        }
        v = 11.0f + (*(float *)(arg0 + 0x8C) + (*(float *)(arg1 + 0xA4) + (*(float *)(arg0 + 0x5C) - *(float *)0x70003A24)));
    } else {
        v = 11.0f + (*(float *)(arg0 + 0x8C) + (*(float *)(arg0 + 0x5C) + *(float *)(arg1 + 0xA4)));
    }
    func_0018C5A0(arg0 + 0x10, v, 4.0f);
}
