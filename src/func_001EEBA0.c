// NEARMISS func_001EEBA0  (vram 0x001EEBA0, 0x30C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 97.91% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Two permuter-class residuals (NOT the clean-store nop): (1) the middle of three identical call sites has its float-constant arg loads (1.0f / 9.999e-7f / 10.0f) scheduled in a different order by mwcc -> instruction-scheduling permutation; (2) the trailing clamp block is instruction-identical but ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// objdiff 97.9% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4);
// the pinned 991202 build reaches 89.6%. The LOGIC and STRUCTURE are fully
// recovered and instruction order is byte-identical except for the residuals
// noted below (compiler scheduling / FP-register-coloring artifacts).
//
// Initializes two 8-float global parameter blocks (D_00257380[], D_002574A0[]),
// then runs three identical "emit particle/clip" iterations: each advances the
// PRNG word at D_00275C34[1] (r = r*0x25 + 0xB), derives a normalized float
// f = ((r>>16)&0xFFFF)/65535 + 0.0001, and calls func_001CFB50(D_0081F8F0, 0,
// arg0, *(float*)(D_00275C34+0x54), f, 1.0f, 9.999999e-07f, 10.0f) followed by
// func_001CFBE0(arg1, 1, <table>, D_0081F8F0, 1) with table D_00257360 /
// D_00257870 / D_00257480 for iterations 1/2/3. Finally it relaxes the float at
// D_00275C34+8 toward 0.02 by 1/10 each call, then floors it up to 0.02.
// (Twin of func_001EAD70, which matches 100%, but with 3 iterations + the header
// block + trailing clamp.)
//
// RESIDUAL (permuter-class, NOT the clean-store nop 2.3.3 fixes):
//  (1) middle iteration: mwcc schedules the float-constant arg loads (1.0f vs
//      9.999e-7f vs 10.0f) in a different order than the boot for that one of the
//      three identical call sites -> instruction-scheduling permutation.
//  (2) trailing clamp block: identical instruction sequence, but mwcc colors the
//      FP temporaries ft0/ft1 where the boot uses fv0/fv1 -> FP-register-coloring.
//  (3) header stores: byte-identical order; objdiff only flags the data-symbol
//      granularity (boot has per-field symbols D_00257384.., this indexes one
//      array D_00257380[]) -> same final addresses, cosmetic.
extern int *D_00275C34;
extern int D_0081F8F0[2];
extern float D_00257380[8];
extern float D_002574A0[8];
extern int D_00257360[2];
extern int D_00257870[2];
extern int D_00257480[2];

void func_001CFB50(void *p, int b, int c, float f0, float f1, float f2, float f3, float f4);
void func_001CFBE0(int a, int b, void *c, void *d, int e);

void func_001EEBA0(int arg0, int arg1) {
    int r;
    float f;

    D_00257380[0] = 8.0f;
    D_00257380[1] = 24.0f;
    D_00257380[2] = 8.0f;
    D_00257380[3] = 0.0f;
    D_00257380[4] = 0.0f;
    D_00257380[5] = 0.0f;
    D_00257380[6] = 0.0f;
    D_00257380[7] = 0.0f;
    D_002574A0[0] = 24.0f;
    D_002574A0[1] = 112.0f;
    D_002574A0[2] = 24.0f;
    D_002574A0[3] = 128.0f;
    D_002574A0[4] = 24.0f;
    D_002574A0[5] = 112.0f;
    D_002574A0[6] = 24.0f;
    D_002574A0[7] = 0.0f;

    r = D_00275C34[1];
    f = (float)((r >> 0x10) & 0xFFFF);
    f /= 65535.0f;
    D_00275C34[1] = r * 0x25 + 0xB;
    f += 0.0001f;
    func_001CFB50(D_0081F8F0, 0, arg0,
                  *(float *)((char *)D_00275C34 + 0x54),
                  f, 1.0f, 9.999999974752427e-07f, 10.0f);
    func_001CFBE0(arg1, 1, D_00257360, D_0081F8F0, 1);

    r = D_00275C34[1];
    f = (float)((r >> 0x10) & 0xFFFF);
    f /= 65535.0f;
    D_00275C34[1] = r * 0x25 + 0xB;
    f += 0.0001f;
    func_001CFB50(D_0081F8F0, 0, arg0,
                  *(float *)((char *)D_00275C34 + 0x54),
                  f, 1.0f, 9.999999974752427e-07f, 10.0f);
    func_001CFBE0(arg1, 1, D_00257870, D_0081F8F0, 1);

    r = D_00275C34[1];
    f = (float)((r >> 0x10) & 0xFFFF);
    f /= 65535.0f;
    D_00275C34[1] = r * 0x25 + 0xB;
    f += 0.0001f;
    func_001CFB50(D_0081F8F0, 0, arg0,
                  *(float *)((char *)D_00275C34 + 0x54),
                  f, 1.0f, 9.999999974752427e-07f, 10.0f);
    func_001CFBE0(arg1, 1, D_00257480, D_0081F8F0, 1);

    {
        float v;
        v = *(float *)((char *)D_00275C34 + 8);
        v += (0.02f - v) / 10.0f;
        *(float *)((char *)D_00275C34 + 8) = v;
        v = *(float *)((char *)D_00275C34 + 8);
        if (v < 0.02f) {
            v = 0.02f;
        }
        *(float *)((char *)D_00275C34 + 8) = v;
    }
}
