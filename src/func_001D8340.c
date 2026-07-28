// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Camera/rig shake-force accumulator update (5 args: a0=owner entity, a1/a2
// unused, a3=live flag, t0=reference vector for the probe builder). When the
// live flag is clear, zeroes the two accumulator quads of the global rig block
// at D_00275688 (+0xF0..FC scale/decay and +0xC0..CC orientation). Then for
// each of the 3 rig slots (stride 0x10): builds a rotation matrix from the
// euler triple at g+i*0x10+0x80 (Y from +0, Z from +4, X from +8 via
// func_00102B08/func_00102BB0/func_00102A60 on an identity from
// func_001029C0) and an identity quat {0,1,0,1}; slot 0 with the flag set
// composes the D_00810610 matrix (inverted via func_00102798) with the
// rotated quat (w forced 0) into g+0xC0 and sets its w to 1.0, with the flag
// clear zeroes both the +0x80 and +0xC0 quads; slots 1-2 always compose the
// rotation into their +0xC0 quad. Second half only when the owner exists and
// func_001D8270(owner) passes: seeds two qword accumulators from
// D_00253170/D_00253180, adds the g+0xC0 vector scaled by *(float*)(g+0xFC)
// (func_00102900 + func_001028B8), snapshots g+0xF0; then scans the 32-entry
// 0x80-stride table at D_00275670 — every entry with weight (+0x24C) > 0
// contributes: probe = func_001028D0(ent+0x10, refvec), len = |probe| clamped
// >= 1, f = (0.1 * ent[+0x2C]) / len, accumulates the 10*f-scaled transformed
// probe (through ent+0x40 via func_001026A0) and the 2*f-scaled ent+0x20
// vector. Finally normalizes the first accumulator (func_00102760) into
// g+0xC0 and copies the second into g+0xF0.
//
// Matched with mwcc 2.3.3 (mwcps2-2.3.3-000906); the pinned 991202 build
// reaches 88.0%. Keys: strength-reduced i*0x10 / j*0x80 indices (no separate
// offset variable) so the derived IV takes the highest saved reg; fresh
// j/ent locals for the second loop; the 16-byte seeds as STRUCT assignments
// (*(V16 *)acc = ...) so the source lq evaluates before the destination
// address (uint128 scalar assignment folds to sp-relative sq and mismatches);
// `ent = base + 0x220;` hoisted above the weight guard so its addiu fills the
// bc1t slot; `qa[3] = 0.0f;` written last before func_001026A0 so the store
// sinks into the jal slot. objdiff 100.0 vs build/expected/func_001D8340.o.
typedef unsigned __int128 uint128;
typedef struct V16 { uint128 q; } V16;

extern void copy_qw4(void *dst, void *src);
extern void func_001026A0(void *dst, void *a, void *b);
extern float func_00102738(void *a, void *b);
extern void func_00102760(void *dst, void *src);
extern void func_00102798(void *dst, void *src);
extern void func_001028B8(void *dst, void *a, void *b);
extern void func_001028D0(void *dst, void *a, void *b);
extern void func_00102900(void *dst, void *src, float f);
extern void func_00102948(void *dst, void *src);
extern void func_001029C0(void *m);
extern void func_00102A60(void *dst, void *src, float f);
extern void func_00102B08(void *dst, void *src, float f);
extern void func_00102BB0(void *dst, void *src, float f);
extern int func_001D8270(char *p);

extern char D_00253170[16];
extern char D_00253180[16];
extern char *D_00275670;
extern char *D_00275688;
extern char D_00810610[64];

void func_001D8340(char *arg0, int arg1, int arg2, int arg3, char *arg4) {
    int i;
    int j;
    char *ent;
    char *base;
    char *s1;
    char *s2;
    float len;
    float f20;
    float qa[4];
    float rotm[16];
    float im[16];
    float res[4];
    float tmp[4];
    char acc[16];
    char wacc[16];
    float probe[4];

    if (arg3 == 0) {
        *(int *)(D_00275688 + 0xFC) = 0;
        *(int *)(D_00275688 + 0xF8) = 0;
        *(int *)(D_00275688 + 0xF4) = 0;
        *(int *)(D_00275688 + 0xF0) = 0;
        *(int *)(D_00275688 + 0xC0) = 0;
        *(int *)(D_00275688 + 0xC4) = 0;
        *(int *)(D_00275688 + 0xC8) = 0;
        *(int *)(D_00275688 + 0xCC) = 0;
    }
    for (i = 0; i < 3; i++) {
        base = D_00275688 + i * 0x10;
        s1 = base + 0x80;
        s2 = base + 0xC0;
        func_001029C0(rotm);
        func_00102B08(rotm, rotm, *(float *)(s1 + 0));
        func_00102A60(rotm, rotm, *(float *)(s1 + 8));
        func_00102BB0(rotm, rotm, *(float *)(s1 + 4));
        qa[0] = 0.0f;
        qa[1] = 1.0f;
        qa[2] = 0.0f;
        qa[3] = 1.0f;
        if (i == 0) {
            if (arg3 != 0) {
                copy_qw4(im, D_00810610);
                func_00102798(im, im);
                func_001026A0(qa, rotm, qa);
                qa[3] = 0.0f;
                func_001026A0(s2, im, qa);
                *(float *)(s2 + 0xC) = 1.0f;
            } else {
                *(float *)(s2 + 0x0) = 0.0f;
                *(float *)(s2 + 0x4) = 0.0f;
                *(float *)(s2 + 0x8) = 0.0f;
                *(float *)(s2 + 0xC) = 0.0f;
                *(float *)(s1 + 0x0) = 0.0f;
                *(float *)(s1 + 0x4) = 0.0f;
                *(float *)(s1 + 0x8) = 0.0f;
                *(float *)(s1 + 0xC) = 0.0f;
            }
        } else {
            func_001026A0(s2, rotm, qa);
        }
    }
    if (arg0 == 0) {
        return;
    }
    if (func_001D8270(arg0) == 0) {
        return;
    }
    *(V16 *)acc = *(V16 *)D_00253170;
    *(V16 *)wacc = *(V16 *)D_00253180;
    func_00102900(tmp, D_00275688 + 0xC0, *(float *)(D_00275688 + 0xFC));
    func_001028B8(acc, acc, tmp);
    func_00102948(wacc, D_00275688 + 0xF0);
    for (j = 0; j < 0x20; j++) {
        base = D_00275670 + j * 0x80;
        ent = base + 0x220;
        if (!(*(float *)(base + 0x24C) <= 0.0f)) {
            func_001028D0(probe, ent + 0x10, arg4);
            len = func_00102738(probe, probe);
            if (len < 1.0f) {
                len = 1.0f;
            }
            f20 = (0.1f * *(float *)(ent + 0x2C)) / len;
            func_00102900(tmp, probe, 10.0f * f20);
            func_001026A0(tmp, ent + 0x40, tmp);
            func_001028B8(acc, acc, tmp);
            func_00102900(tmp, ent + 0x20, 2.0f * f20);
            func_001028B8(wacc, wacc, tmp);
        }
    }
    func_00102760(res, acc);
    func_00102948(D_00275688 + 0xC0, res);
    func_00102948(D_00275688 + 0xF0, wacc);
}
