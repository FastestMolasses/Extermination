// NEARMISS func_001E0E80  (vram 0x001E0E80, 0x190 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 86.12% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-coloring near-miss (permuter territory). Body, control flow, stack layout (sp50 at 0x50, the float vector at 0x90), gp-rel global (-sdatathreshold 8), branch senses (the inverted 'if(!(v[1]<0))' early-return, the branch-likely 'if(t>=1.0)var=t', the bgez/slti clamp) and the float-vector-...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

//
// NEARMISS (86.12% mwcc233; logic/structure/stack-layout byte-identical, residual =
// arg0/arg1 callee-saved register coloring swap -- register-allocation-order wall).
//
// Builds a 4-float scratch vector v[] on the stack (v[0]=(float)arg1, v[1]=(float)arg2,
// v[2]=global D_00275670[0x2468], v[3]=0) after func_00102798 inits a 0x40-byte work
// buffer (sp50) from D_00275670+0x2380, then func_001026A0 transforms v in place.
// If v[1] (the arg2 component) is >= 0, writes a zero/zero/1.0 result to *arg0 and
// returns 0. Otherwise scales a clamped factor var_f20 = max(1.0, D_008105D4-10) and
// writes 0.002*v[0]*t, 0.002*v[2]*t, 1.0 to *arg0 (t = (D_008105D4-10)/v[1]), returning
// a clamped intensity float_to_int(256*func_0011DF78(v[1]) / var_f20) bounded to [0,0x80].
extern int float_to_int(float);
extern void func_001026A0(float *, void *, float *);
extern void func_00102798(void *, char *);
extern float func_0011DF78(float);
extern char *D_00275670;
extern float D_008105D4;

int func_001E0E80(char *arg0, int arg1, int arg2) {
    char sp50[0x40];
    float v[4];
    float t;
    float var_f20;
    int v0;

    func_00102798(sp50, D_00275670 + 0x2380);
    v[0] = (float) arg1;
    v[1] = (float) arg2;
    v[2] = *(float *)(D_00275670 + 0x2468);
    *(int *)&v[3] = 0;
    func_001026A0(&v[0], sp50, &v[0]);
    if (!(v[1] < 0.0f)) {
        *(float *)(arg0 + 0) = 0.0f;
        *(float *)(arg0 + 4) = 0.0f;
        *(int *)(arg0 + 8) = 0x3F800000;
        return 0;
    }
    var_f20 = 1.0f;
    t = D_008105D4 - 10.0f;
    if (t >= 1.0f) {
        var_f20 = t;
    }
    t = t / v[1];
    *(float *)(arg0 + 0) = 0.002f * (v[0] * t);
    *(float *)(arg0 + 4) = 0.002f * (v[2] * t);
    *(int *)(arg0 + 8) = 0x3F800000;
    v0 = float_to_int((256.0f * func_0011DF78(v[1])) / var_f20);
    if (v0 < 0) {
        return 0;
    }
    if (v0 >= 0x81) {
        v0 = 0x80;
    }
    return v0;
}
