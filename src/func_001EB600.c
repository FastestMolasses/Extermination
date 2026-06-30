// NEARMISS func_001EB600  (vram 0x001EB600, 0x1F0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 89.90% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FP register-coloring + float-constant scheduling in the two func_001CFB50 arg setups. Body/structure fully recovered and identical to the matched sibling family (func_001EB7F0/func_001EBA20 at -O4,p -sdatathreshold 4); the prologue additions unique to this variant (fade-guard, func_0021B9A0 brack...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// Particle/effect spawn driver, same idiom family as the matched siblings
// func_001EB7F0 / func_001EBA20. Bracketed by audio/channel calls
// func_0021B9A0(chan, vol, pan): chan 2 then 3 (pan 2000.0f) before, chan 1
// after. Stores 20.0f to the object's +0x34 field, and decays its +0x38 field
// by 3.0 when the gp-relative fade scalar *(D_00275C34+0x54) is > 0. Then fires
// two identical passes: read the 32-bit LCG seed at *(D_00275C34+0x04), turn
// its high 16 bits into a [0,1) fraction (/65535.0f + 0.0001f bias), advance
// the seed (seed = seed*0x25 + 0xB), call func_001CFB50 with the shared block
// D_0081F8F0, the fade scalar, that fraction and the tuning constants, and
// queue via func_001CFBE0(arg1, 5, descriptor, block, 0) against D_00255D70
// then D_00255E00.
//
// NEARMISS: 89.9% with mwcc 2.3.3 (991202 = 81.9%). Logic fully recovered;
// residual is FP register-coloring + float-constant scheduling in the
// func_001CFB50 argument setup (permuter class).
extern int *D_00275C34;
extern int D_00255D70[2];
extern int D_00255E00[2];
extern int D_0081F8F0[2];

void func_0021B9A0(int chan, float a, float b);
void func_001CFB50(void *p, int b, int c, float f0, float f1, float f2, float f3, float f4);
void func_001CFBE0(int a, int b, void *c, void *d, int e);

void func_001EB600(int arg0, int arg1) {
    int r;
    float f;

    if (!(*(float *)((char *)D_00275C34 + 0x54) <= 0.0f)) {
        *(float *)(arg0 + 0x38) -= 3.0f;
    }
    func_0021B9A0(2, 0.0f, 0.0f);
    func_0021B9A0(3, 0.0f, 2000.0f);
    *(float *)(arg0 + 0x34) = 20.0f;

    r = D_00275C34[1];
    f = (float)((r >> 0x10) & 0xFFFF);
    f /= 65535.0f;
    D_00275C34[1] = r * 0x25 + 0xB;
    f += 0.0001f;
    func_001CFB50(D_0081F8F0, 0, arg0,
                  *(float *)((char *)D_00275C34 + 0x54),
                  f, 1.0f, 9.999999974752427e-07f, 15.0f);
    func_001CFBE0(arg1, 5, D_00255D70, D_0081F8F0, 0);

    r = D_00275C34[1];
    f = (float)((r >> 0x10) & 0xFFFF);
    f /= 65535.0f;
    D_00275C34[1] = r * 0x25 + 0xB;
    f += 0.0001f;
    func_001CFB50(D_0081F8F0, 0, arg0,
                  *(float *)((char *)D_00275C34 + 0x54),
                  f, 1.0f, 9.999999974752427e-07f, 15.0f);
    func_001CFBE0(arg1, 5, D_00255E00, D_0081F8F0, 0);

    func_0021B9A0(1, 0.0f, 0.0f);
}
