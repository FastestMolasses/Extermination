// NEARMISS func_001EB440  (vram 0x001EB440, 0x1B8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 94.31% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FP register-coloring permutation in the SECOND of two identical spawn passes (the first pass is byte-identical). The CW target colors the fraction/divide/constants into paired odd-half FP regs (fv0f/fa0f/fa1f) and loads the 15.0f tuning constant before 1e-6f, whereas mwcc colors them into separat...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// Brackets two identical particle/effect spawn passes with func_0021B9A0 audio/
// channel calls (chan 2 off, chan 3 set to 2000.0, ... chan 1 off at the end).
// Each pass reads the gp-relative LCG seed at *(D_00275C34+0x04), turns its high
// 16 bits into a [0,1) fraction via /65535.0f + 0.0001f bias, advances the seed
// (seed = seed*0x25 + 0xB), then calls func_001CFB50 with the shared sprite/anim
// block D_0081F8F0, the fade scalar at *(D_00275C34+0x54), the random fraction,
// and the unit/epsilon/15.0 tuning constants; finally queues via func_001CFBE0
// against a per-pass descriptor (D_00255C50 then D_00255CE0). D_00275C34 is
// gp-relative (sdatathreshold 4); D_0081F8F0 / D_00255C50 / D_00255CE0 stay
// absolute. Same idiom family as matched sibling func_001EBA20.
//
// NEARMISS 94.3% (mwcc 2.3.3; 991202 caps 79.1%) — logic fully recovered.
// The FIRST spawn pass is byte-identical. Sole residual is FP register coloring
// in the SECOND pass: the CW target colors the fraction/divide/constants into
// the paired odd-half FP regs (fv0f/fa0f/fa1f) and loads the 15.0f tuning
// constant before 1e-6f, whereas mwcc colors them into separate single regs
// (fv1/fa1/fa3) and reverses the two constant loads. Driven by the register
// pressure from the func_0021B9A0 bracketing calls (the matched sibling
// func_001EBA20 has no such calls and reaches 100.0). Body/structure/loads/math
// all correct; the divergence is a pure FP-coloring permutation -> permuter
// class, not the clean-store nop. Documented scheduling/coloring wall.
extern int *D_00275C34;
extern int D_00255C50[2];
extern int D_00255CE0[2];
extern int D_0081F8F0[2];

void func_0021B9A0(int chan, float a, float b);
void func_001CFB50(void *p, int b, int c, float f0, float f1, float f2, float f3, float f4);
void func_001CFBE0(int a, int b, void *c, void *d, int e);

void func_001EB440(int arg0, int arg1) {
    int r;
    float f;

    func_0021B9A0(2, 0.0f, 0.0f);
    func_0021B9A0(3, 0.0f, 2000.0f);

    r = D_00275C34[1];
    f = (float)((r >> 0x10) & 0xFFFF);
    f /= 65535.0f;
    D_00275C34[1] = r * 0x25 + 0xB;
    f += 0.0001f;
    func_001CFB50(D_0081F8F0, 0, arg0,
                  *(float *)((char *)D_00275C34 + 0x54),
                  f, 1.0f, 9.999999974752427e-07f, 15.0f);
    func_001CFBE0(arg1, 5, D_00255C50, D_0081F8F0, 0);

    r = D_00275C34[1];
    f = (float)((r >> 0x10) & 0xFFFF);
    f /= 65535.0f;
    D_00275C34[1] = r * 0x25 + 0xB;
    f += 0.0001f;
    func_001CFB50(D_0081F8F0, 0, arg0,
                  *(float *)((char *)D_00275C34 + 0x54),
                  f, 1.0f, 9.999999974752427e-07f, 15.0f);
    func_001CFBE0(arg1, 5, D_00255CE0, D_0081F8F0, 0);

    func_0021B9A0(1, 0.0f, 0.0f);
}
