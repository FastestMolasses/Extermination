// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 86.12% NEARMISS).
// Fixes read from the original instructions:
// - D_008105D4 is reached by absolute address, not gp-relative: declared as an
//   unsized array so -sdatathreshold 8 keeps only D_00275670 in small data.
// - The clamp is a ternary `(1.0f > t) ? 1.0f : t`, not an if-assignment.
// - v[3] is stored as a float zero.
// - The quotient (D_008105D4 - 10) / v[1] is its own local, and v[2] * q is
//   computed before v[0] * q (the old "argument register swap" came from these).
// - The final clamp is the strict compare `> 0x80`.
//
// Builds a 4-float scratch vector v[] on the stack (v[0]=(float)arg1, v[1]=(float)arg2,
// v[2]=global D_00275670[0x2468], v[3]=0) after func_00102798 inits a 0x40-byte work
// buffer (sp50) from D_00275670+0x2380, then func_001026A0 transforms v in place.
// If v[1] (the arg2 component) is >= 0, writes a zero/zero/1.0 result to *arg0 and
// returns 0. Otherwise uses a clamped factor var_f20 = max(1.0, D_008105D4-10) and
// writes 0.002*v[0]*q, 0.002*v[2]*q, 1.0 to *arg0 (q = (D_008105D4-10)/v[1]), returning
// a clamped intensity float_to_int(256*func_0011DF78(v[1]) / var_f20) bounded to [0,0x80].
extern int float_to_int(float);
extern void func_001026A0(float *, void *, float *);
extern void func_00102798(void *, char *);
extern float func_0011DF78(float);
extern char *D_00275670;
extern float D_008105D4[];

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
    v[3] = 0.0f;
    func_001026A0(&v[0], sp50, &v[0]);
    if (!(v[1] < 0.0f)) {
        *(float *)(arg0 + 0) = 0.0f;
        *(float *)(arg0 + 4) = 0.0f;
        *(int *)(arg0 + 8) = 0x3F800000;
        return 0;
    }
    t = D_008105D4[0] - 10.0f;
    var_f20 = (1.0f > t) ? 1.0f : t;
    {
        float q = t / v[1];
        float z = v[2] * q;
        *(float *)(arg0 + 0) = 0.002f * (v[0] * q);
        *(float *)(arg0 + 4) = 0.002f * z;
    }
    *(int *)(arg0 + 8) = 0x3F800000;
    v0 = float_to_int((256.0f * func_0011DF78(v[1])) / var_f20);
    if (v0 < 0) {
        return 0;
    }
    if (v0 > 0x80) {
        v0 = 0x80;
    }
    return v0;
}
