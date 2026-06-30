// NEARMISS func_0018C5A0  (vram 0x0018C5A0, 0xF4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 92.21% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Branch-likely SENSE (target bc1tl vs mwcc bc1fl) on the min-clamp, plus FP register coloring (target keeps t/8 and arg13 in the f0/f20 odd-half pair fv0f/fs0f; mwcc colors them fv1/fs1) and return-tail scheduling (paddub v0 placement). Body/control-flow fully recovered; the residual is a CW-2.3.1...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Float approach/ease driver on the field at arg0+4. Computes
// diff = (arg12 + D_00810278) - field, then t = func_0011DF78(diff) (|diff| or
// a magnitude). If t > 1.0: step = min(t/8, arg13), negated when diff < 0, is
// added to the field and 0 is returned. Otherwise (t <= 1.0): the field is
// nudged by diff/4 and 4 is returned (a "close enough / snap" result code).
//
// NEARMISS 92.2% (mwcc 2.3.3; 991202 = 82.4%). Body and control flow are
// correct -- the target lays the t>1 path as the fall-through and the t<=1
// /4 path as the forward block, and emits the min-clamp as a branch-likely
// with a dead mov tail, all of which this C reproduces. The residual is a
// branch-likely SENSE difference (target bc1tl vs mwcc bc1fl) plus FP register
// coloring (target uses the f0/f20 odd-half pair; mwcc uses f1/f21) and the
// return-value paddub scheduling -- pure compiler-version codegen artifacts
// (permuter class), not a logic gap.
extern float D_00810278;
extern float func_0011DF78(float x);

int func_0018C5A0(char *arg0, float arg12, float arg13) {
    char *self = arg0;
    float a13 = arg13;
    float diff;
    float t;
    float v;

    arg12 = arg12 + D_00810278;
    diff = arg12 - *(float *)(self + 4);
    t = func_0011DF78(diff);
    if (t > 1.0f) {
        v = t / 8.0f;
        v = (v <= a13) ? v : a13;
        if (diff < 0.0f)
            v = -v;
        *(float *)(self + 4) = *(float *)(self + 4) + v;
        return 0;
    }
    *(float *)(self + 4) = *(float *)(self + 4) + diff / 4.0f;
    return 4;
}
