// NEARMISS func_001ED7A0  (vram 0x001ED7A0, 0x34C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 95.73% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FP-constant register-coloring permutation in the func_001CFB50 call setup -- same class and same fix-set as its twin func_001ED450 (gp-rel D_00275C34 via -sdatathreshold 4; absolute in-order init block via `float[2]` + `volatile`). Body/structure fully recovered (95.7% via mwcc 2.3.3). Sole resid...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// NEARMISS func_001ED7A0 -- readable decompilation, body/structure faithful; NOT byte-identical.
// objdiff 95.7% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4); pinned 991202 = 86.1%.
// Near-identical twin of func_001ED450 (different descriptor blocks/constants). Sole residual
// is the same compiler artifact: the three FP constant args (1.0f, 9.99e-7f, 10.0f) to
// func_001CFB50 color to $fa2/$fa3/$fa4 here vs the target's $fa1f/$ft4/$fa1 -- an FP
// register-coloring permutation. Same proven keys as the twin: D_00275C34 gp-relative
// (-sdatathreshold 4, `int*` indexed [1]); the 0x257380/410/4A0 init words declared `float[2]`
// so they stay absolute with per-word relocs; `volatile` forces the 24 init stores in address
// order instead of mwcc hoisting the zero-stores.
//
// Periodic particle/effect re-seed (sibling of func_001ED450): rewrites three 8-word (0x20)
// global descriptor blocks at D_00257380, D_00257410, D_002574A0 to their constant defaults
// (16/12/16/0... and 128/0/128/96/96/0/96/0 twice), then for each of the three sub-emitters
// advances the global LCG D_00275C34[1] (state = state*0x25 + 0xB), derives the normalized
// random scalar f = ((state>>16 & 0xFFFF)/65535) + 0.0001, primes the shared param buffer
// D_0081F8F0 via func_001CFB50(buf, 0, arg0, timer, f, 1.0, 9.99e-7, 10.0), and submits the
// emitter for table D_00257360 / D_002573F0 / D_00257480 via func_001CFBE0(arg1, 1, table,
// buf, 1). Finally eases the global fade D_00275C34+8 toward 0.02 by a tenth each call and
// clamps it to a 0.02 floor.
extern int *D_00275C34;
extern int D_00257360[2];
extern int D_002573F0[2];
extern int D_00257480[2];
extern int D_0081F8F0[2];
extern volatile float D_00257380[2], D_00257384[2], D_00257388[2], D_0025738C[2], D_00257390[2], D_00257394[2], D_00257398[2], D_0025739C[2];
extern volatile float D_00257410[2], D_00257414[2], D_00257418[2], D_0025741C[2], D_00257420[2], D_00257424[2], D_00257428[2], D_0025742C[2];
extern volatile float D_002574A0[2], D_002574A4[2], D_002574A8[2], D_002574AC[2], D_002574B0[2], D_002574B4[2], D_002574B8[2], D_002574BC[2];

void func_001CFB50(void *p, int b, int c, float f0, float f1, float f2, float f3, float f4);
void func_001CFBE0(int a, int b, void *c, void *d, int e);

void func_001ED7A0(int arg0, int arg1) {
    int r;
    float f;
    float v;

    D_00257380[0] = 16.0f; D_00257384[0] = 12.0f; D_00257388[0] = 16.0f;
    D_0025738C[0] = 0.0f; D_00257390[0] = 0.0f; D_00257394[0] = 0.0f; D_00257398[0] = 0.0f; D_0025739C[0] = 0.0f;
    D_00257410[0] = 128.0f; D_00257414[0] = 0.0f; D_00257418[0] = 128.0f; D_0025741C[0] = 96.0f; D_00257420[0] = 96.0f; D_00257424[0] = 0.0f; D_00257428[0] = 96.0f; D_0025742C[0] = 0.0f;
    D_002574A0[0] = 128.0f; D_002574A4[0] = 0.0f; D_002574A8[0] = 128.0f; D_002574AC[0] = 96.0f; D_002574B0[0] = 96.0f; D_002574B4[0] = 0.0f; D_002574B8[0] = 96.0f; D_002574BC[0] = 0.0f;

    r = D_00275C34[1];
    f = (float)((r >> 0x10) & 0xFFFF);
    f /= 65535.0f;
    D_00275C34[1] = r * 0x25 + 0xB;
    f += 0.0001f;
    func_001CFB50(D_0081F8F0, 0, arg0, *(float *)((char *)D_00275C34 + 0x54), f, 1.0f, 9.999999974752427e-07f, 10.0f);
    func_001CFBE0(arg1, 1, D_00257360, D_0081F8F0, 1);

    r = D_00275C34[1];
    f = (float)((r >> 0x10) & 0xFFFF);
    f /= 65535.0f;
    D_00275C34[1] = r * 0x25 + 0xB;
    f += 0.0001f;
    func_001CFB50(D_0081F8F0, 0, arg0, *(float *)((char *)D_00275C34 + 0x54), f, 1.0f, 9.999999974752427e-07f, 10.0f);
    func_001CFBE0(arg1, 1, D_002573F0, D_0081F8F0, 1);

    r = D_00275C34[1];
    f = (float)((r >> 0x10) & 0xFFFF);
    f /= 65535.0f;
    D_00275C34[1] = r * 0x25 + 0xB;
    f += 0.0001f;
    func_001CFB50(D_0081F8F0, 0, arg0, *(float *)((char *)D_00275C34 + 0x54), f, 1.0f, 9.999999974752427e-07f, 10.0f);
    func_001CFBE0(arg1, 1, D_00257480, D_0081F8F0, 1);

    v = *(float *)((char *)D_00275C34 + 8);
    v += (0.02f - v) / 10.0f;
    *(float *)((char *)D_00275C34 + 8) = v;
    v = *(float *)((char *)D_00275C34 + 8);
    if (v < 0.02f) { v = 0.02f; }
    *(float *)((char *)D_00275C34 + 8) = v;
}
