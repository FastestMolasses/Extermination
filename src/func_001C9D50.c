// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Quat-pose blend: builds two scratch quats from a (arg1) and b (arg2) via
// func_001C9E40, NLERPs them by `blend` into a third scratch quat, writes the
// resulting 3x4 rotation to `out`, then linearly blends the trans/scale triple
// at +0x30/0x34/0x38: out = a*(1-blend) + b*blend. mwcc 991202 mis-schedules the
// func_001C9E40 arg-setup cluster (83.1%); mwcc 2.3.3 emits the target ordering
// and the fs0/fs0f mula/madd register pairing -> 100.0.
extern void func_001C9E40(float *, char *);
extern void quat_nlerp(float *, float *, float *, float);
extern void quat_to_mat3(char *, float *, float *);
extern float D_700034C0[4];
extern float D_700034D0[4];
extern float D_700034E0[4];

void func_001C9D50(char *out, char *a, char *b, float blend) {
    float w = 1.0f - blend;
    func_001C9E40(D_700034C0, a);
    func_001C9E40(D_700034D0, b);
    quat_nlerp(D_700034E0, D_700034C0, D_700034D0, blend);
    quat_to_mat3(out, D_700034E0, D_700034C0);
    *(float *)(out + 0x30) = *(float *)(a + 0x30) * w + *(float *)(b + 0x30) * blend;
    *(float *)(out + 0x34) = *(float *)(a + 0x34) * w + *(float *)(b + 0x34) * blend;
    *(float *)(out + 0x38) = *(float *)(a + 0x38) * w + *(float *)(b + 0x38) * blend;
}
