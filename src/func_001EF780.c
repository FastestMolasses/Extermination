// NEARMISS func_001EF780  (vram 0x001EF780, 0x1B4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 97.60% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FP register coloring / paired-single allocation + order of materializing the two call-argument float constants (1.0f and 1e-6f). Body and structure fully recovered; residual is a scheduling/coloring permutation, not logic. Permuter territory; 2.3.3 does not fix it.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// NEARMISS (mwcc233 = 97.6%, 991202 = 90.4%): body and structure fully
// recovered; the sole residual is FP register coloring / paired-single
// allocation plus the order in which two call-argument float constants (1.0f
// and 1e-6f) are materialized — a compiler scheduling/coloring permutation,
// not a logic difference. Permuter territory.
//
// Particle/matrix emit loop run 8 times. Each iteration builds a transform in
// the scratch matrix D_700036A0 (identity via func_001029C0, scale by table
// D_00257C50[(i>>1)], compose with per-iteration matrix D_00257BD0[i],
// transform by arg0 via func_001026D0), advances a 32-bit LCG counter at
// D_00275C34[1] (x = x*0x25 + 0xB) and derives a normalized jitter
// ((x>>16 & 0xFFFF)/65535 + 0.0001), then submits a particle via
// func_001CFB50(desc D_0081F8F0, 0, matrix, life=*(D_00275C34+0x54), jitter,
// 1.0, 1e-6, 0.0) and func_001CFBE0(arg1, 1, D_00257B40, D_0081F8F0, 0).
// After the loop, eases the global float *(D_00275C34+8) toward 0.02 by 1/10
// per call and clamps it to a 0.02 floor.
//
// -sdatathreshold 4 puts D_00275C34 (gp-range pointer) gp-relative, matching CW.
extern void func_001029C0(void *m);
extern void func_00102C58(void *dst, void *src, float *v);
extern void func_00102918(void *dst, void *src, void *m);
extern void func_001026D0(void *dst, int n, void *m);
extern void func_001CFB50(void *p, int b, void *c, float f0, float f1, float f2, float f3, float f4);
extern void func_001CFBE0(int a, int b, void *c, void *d, int e);
extern int *D_00275C34;
extern float D_00257B40[];
extern float D_00257BD0[];
extern float D_00257C50[];
extern float D_700036A0[];
extern int D_0081F8F0[];

void func_001EF780(int arg0, int arg1) {
    int i;
    float *pm;
    int r;
    float f;

    pm = D_00257BD0;
    for (i = 0; i < 8; i++) {
        func_001029C0(D_700036A0);
        func_00102C58(D_700036A0, D_700036A0, &D_00257C50[(i >> 1) * 4]);
        func_00102918(D_700036A0, D_700036A0, pm);
        func_001026D0(D_700036A0, arg0, D_700036A0);
        r = D_00275C34[1];
        f = (float)((r >> 0x10) & 0xFFFF);
        f /= 65535.0f;
        D_00275C34[1] = r * 0x25 + 0xB;
        f += 0.0001f;
        func_001CFB50(D_0081F8F0, 0, D_700036A0,
                      *(float *)((char *)D_00275C34 + 0x54),
                      f, 1.0f, 9.999999974752427e-07f, 0.0f);
        func_001CFBE0(arg1, 1, D_00257B40, D_0081F8F0, 0);
        pm += 4;
    }
    f = *(float *)((char *)D_00275C34 + 8);
    f = f + (0.02f - f) / 10.0f;
    *(float *)((char *)D_00275C34 + 8) = f;
    f = *(float *)((char *)D_00275C34 + 8);
    if (f < 0.02f) {
        f = 0.02f;
    }
    *(float *)((char *)D_00275C34 + 8) = f;
}
