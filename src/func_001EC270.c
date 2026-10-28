// NEARMISS func_001EC270  (vram 0x001EC270, 0x180 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 95.31% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful.
// Improved 2026-10-28 (was 66.71%): -sdatathreshold 4 (D_00275C34 is gp-relative); the
// sibling template of func_001EBF10 (fraction /= 65535, seed advance, += 0.0001f); arg0 is a
// pointer passed through; no TU neighbour needed (compiling func_001EC1F0 first scores lower).
// Remaining differences: the second pass only; the original sets up the 5.0f argument first
// (held in a GPR, moved to the FPU after the 1e-6f setup) and reloads D_00275C34 into the
// same register; this C sets 5.0f up last. Pass 1 matches.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// Two particle/effect emissions. Each advances the LCG seed at D_00275C34[1]
// (seed*0x25 + 0xB), derives a [0,1) fraction ((seed>>16)&0xFFFF)/65535 + 0.0001,
// calls func_001CFB50(D_0081F8F0, 0, arg0, D_00275C34->+0x54, fraction, 1.0f,
// 1e-6f, 5.0f), then func_001CFBE0(arg1, 1 then 0, D_00256790 then D_00256820,
// D_0081F8F0, 0).
extern int *D_00275C34;
extern int D_0081F8F0[2];
void func_001CFB50(void *p, int b, void *c, float f0, float f1, float f2, float f3, float f4);
void func_001CFBE0(int a, int b, void *c, void *d, int e);
extern int D_00256790[2];
extern int D_00256820[2];

void func_001EC270(void *arg0, int arg1) {
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
    func_001CFBE0(arg1, 1, D_00256790, D_0081F8F0, 0);

    r = D_00275C34[1];
    f = (float)((r >> 0x10) & 0xFFFF);
    f /= 65535.0f;
    D_00275C34[1] = r * 0x25 + 0xB;
    f += 0.0001f;
    func_001CFB50(D_0081F8F0, 0, arg0,
                  *(float *)((char *)D_00275C34 + 0x54),
                  f, 1.0f, 9.999999974752427e-07f, 5.0f);
    func_001CFBE0(arg1, 0, D_00256820, D_0081F8F0, 0);
}
