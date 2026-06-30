// NEARMISS func_001EEEB0  (vram 0x001EEEB0, 0x30C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 97.91% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Identical residual class to its twin func_001EEBA0 (NOT the clean-store nop): middle-iteration float-constant-load scheduling permutation + FP-register-coloring (ft0/ft1 vs fv0/fv1) in the trailing clamp, plus cosmetic data-symbol granularity in the header float-block stores. Body/structure 100% ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// Twin of func_001EEBA0 (same structure, different global blocks/tables and an
// 8.0f relaxation divisor instead of 10.0f). objdiff 97.9% via mwcc 2.3.3
// (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4); the pinned 991202 build
// reaches 89.6%. Same permuter-class residuals as the twin (middle-iteration
// float-const scheduling, FP-register-coloring in the clamp, cosmetic data-symbol
// granularity in the header). LOGIC and STRUCTURE fully recovered.
//
// Initializes two 8-float global parameter blocks (D_00257530[], D_00257650[]),
// then runs three identical iterations advancing the PRNG word D_00275C34[1]
// (r = r*0x25 + 0xB), deriving f = ((r>>16)&0xFFFF)/65535 + 0.0001, calling
// func_001CFB50(D_0081F8F0, 0, arg0, *(float*)(D_00275C34+0x54), f, 1.0f,
// 9.999999e-07f, 10.0f) then func_001CFBE0(arg1, 1, <table>, D_0081F8F0, 1) with
// tables D_00257510 / D_00257900 / D_00257630. Finally relaxes the float at
// D_00275C34+8 toward 0.02 by 1/8 each call, then floors it up to 0.02.
extern int *D_00275C34;
extern int D_0081F8F0[2];
extern float D_00257530[8];
extern float D_00257650[8];
extern int D_00257510[2];
extern int D_00257900[2];
extern int D_00257630[2];

void func_001CFB50(void *p, int b, int c, float f0, float f1, float f2, float f3, float f4);
void func_001CFBE0(int a, int b, void *c, void *d, int e);

void func_001EEEB0(int arg0, int arg1) {
    int r;
    float f;

    D_00257530[0] = 8.0f;
    D_00257530[1] = 24.0f;
    D_00257530[2] = 8.0f;
    D_00257530[3] = 0.0f;
    D_00257530[4] = 0.0f;
    D_00257530[5] = 0.0f;
    D_00257530[6] = 0.0f;
    D_00257530[7] = 0.0f;
    D_00257650[0] = 24.0f;
    D_00257650[1] = 112.0f;
    D_00257650[2] = 24.0f;
    D_00257650[3] = 128.0f;
    D_00257650[4] = 24.0f;
    D_00257650[5] = 112.0f;
    D_00257650[6] = 24.0f;
    D_00257650[7] = 0.0f;

    r = D_00275C34[1];
    f = (float)((r >> 0x10) & 0xFFFF);
    f /= 65535.0f;
    D_00275C34[1] = r * 0x25 + 0xB;
    f += 0.0001f;
    func_001CFB50(D_0081F8F0, 0, arg0,
                  *(float *)((char *)D_00275C34 + 0x54),
                  f, 1.0f, 9.999999974752427e-07f, 10.0f);
    func_001CFBE0(arg1, 1, D_00257510, D_0081F8F0, 1);

    r = D_00275C34[1];
    f = (float)((r >> 0x10) & 0xFFFF);
    f /= 65535.0f;
    D_00275C34[1] = r * 0x25 + 0xB;
    f += 0.0001f;
    func_001CFB50(D_0081F8F0, 0, arg0,
                  *(float *)((char *)D_00275C34 + 0x54),
                  f, 1.0f, 9.999999974752427e-07f, 10.0f);
    func_001CFBE0(arg1, 1, D_00257900, D_0081F8F0, 1);

    r = D_00275C34[1];
    f = (float)((r >> 0x10) & 0xFFFF);
    f /= 65535.0f;
    D_00275C34[1] = r * 0x25 + 0xB;
    f += 0.0001f;
    func_001CFB50(D_0081F8F0, 0, arg0,
                  *(float *)((char *)D_00275C34 + 0x54),
                  f, 1.0f, 9.999999974752427e-07f, 10.0f);
    func_001CFBE0(arg1, 1, D_00257630, D_0081F8F0, 1);

    {
        float v;
        v = *(float *)((char *)D_00275C34 + 8);
        v += (0.02f - v) / 8.0f;
        *(float *)((char *)D_00275C34 + 8) = v;
        v = *(float *)((char *)D_00275C34 + 8);
        if (v < 0.02f) {
            v = 0.02f;
        }
        *(float *)((char *)D_00275C34 + 8) = v;
    }
}
