// NEARMISS func_001EC5F0  (vram 0x001EC5F0, 0x22C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 95.32% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FP-constant scheduling permutation isolated to the MIDDLE (2nd) of three identical passes: target stages the 5.0f const (0x40A00000) early via a3/mtc1 interleaved with the LCG seed math; mwcc233 routes it late through v1. Passes 1 and 3 match exactly. Body/logic 100% faithful, CW instruction-sche...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// NEARMISS 95.32% (mwcc233). Spawns three particle/effect packets. Each pass
// advances the shared LCG seed at D_00275C34[1] (s = s*0x25 + 0xB), derives a
// random 0..1 float ((s>>16 & 0xFFFF)/65535 + 0.0001), builds a packet in
// D_0081F8F0 via func_001CFB50(&pkt, 0, arg0, camFloat[0x54], rand, 1.0f,
// 1e-6f, 5.0f), then submits it via func_001CFBE0(arg1, 0, &table, &pkt, 0)
// against a per-pass table (D_00256A60 / D_00256AF0 / D_00256B80).
// RESIDUAL WALL: FP-constant scheduling permutation, isolated to the MIDDLE
// (2nd) pass only - the target stages the 5.0f const (0x40A00000) early
// through a3/mtc1 interleaved with the seed math, while mwcc233 routes it
// late through v1. Passes 1 and 3 match; body/logic fully faithful. Not a
// source-level fix (CW instruction scheduler artifact).
extern int *D_00275C34;
extern int D_00256A60[2];
extern int D_00256AF0[2];
extern int D_00256B80[2];
extern int D_0081F8F0[2];

void func_001CFB50(void *p, int b, int c, float f0, float f1, float f2, float f3, float f4);
void func_001CFBE0(int a, int b, void *c, void *d, int e);

void func_001EC5F0(int arg0, int arg1) {
    int r;
    float f;

    r = D_00275C34[1];
    f = (float)((r >> 0x10) & 0xFFFF);
    f /= 65535.0f;
    D_00275C34[1] = r * 0x25 + 0xB;
    f += 0.0001f;
    func_001CFB50(D_0081F8F0, 0, arg0,
                  *(float *)((char *)D_00275C34 + 0x54),
                  f, 1.0f, 9.999999974752427e-07f, 5.0f);
    func_001CFBE0(arg1, 0, D_00256A60, D_0081F8F0, 0);

    r = D_00275C34[1];
    f = (float)((r >> 0x10) & 0xFFFF);
    f /= 65535.0f;
    D_00275C34[1] = r * 0x25 + 0xB;
    f += 0.0001f;
    func_001CFB50(D_0081F8F0, 0, arg0,
                  *(float *)((char *)D_00275C34 + 0x54),
                  f, 1.0f, 9.999999974752427e-07f, 5.0f);
    func_001CFBE0(arg1, 0, D_00256AF0, D_0081F8F0, 0);

    r = D_00275C34[1];
    f = (float)((r >> 0x10) & 0xFFFF);
    f /= 65535.0f;
    D_00275C34[1] = r * 0x25 + 0xB;
    f += 0.0001f;
    func_001CFB50(D_0081F8F0, 0, arg0,
                  *(float *)((char *)D_00275C34 + 0x54),
                  f, 1.0f, 9.999999974752427e-07f, 5.0f);
    func_001CFBE0(arg1, 0, D_00256B80, D_0081F8F0, 0);
}
