// NEARMISS func_001EC270  (vram 0x001EC270, 0x180 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 66.71% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FP register-coloring + instruction-scheduling near-miss (82.18% mwcc991). All addressing modes (gp-rel D_00275C34, %hi/%lo for large globals), the LCG, the float jitter formula and both call sites match; residual is constant/division FP-register coloring (CW fv0f/fa1/fa1f vs mwcc fv1/fa2/fa3/ft0)...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

//
// NEARMISS 82.18% (mwcc 991202; 233 = 66.71% due to a worse prologue/
// addressing schedule). Body/logic fully recovered. Residual is entirely
// FP register-coloring + minor delay-slot/arg-setup scheduling:
//  * the float constants 1.0f/1e-6f/5.0f and the division result land in
//    different FP registers than CW (target fv0f/fa1/fa1f vs mwcc
//    fv1/fa2/fa3/ft0) -- consistent structural coloring offset.
//  * div.s placement and the 5.0f arg-setup (lui 0x40a0/mtc1) are
//    scheduled one slot apart from CW.
// Requires -sdatathreshold 8 so D_00275C34 is gp-rel (matches
// 'lw %gp_rel(D_00275C34)(gp)') while the larger globals D_0081F8F0/
// D_00256790/820 stay %hi/%lo -- modeled by sizing them [16] so they
// exceed the small-data threshold.
//
// Semantics: two near-identical particle/effect emissions. Each pulls a
// 32-bit value from D_00275C34[1] (offset 0x4), advances it by the LCG
// x*0x25+0xB, derives a normalized float ((v>>16)&0xffff)/65535 + 1e-4 as
// a spread/jitter factor, then calls func_001CFB50 to populate a particle
// descriptor at D_0081F8F0 (args: id 0, kind=arg0, base=*(D_00275C34+0x54),
// jitter, 1.0, 1e-6, 5.0) and func_001CFBE0 to spawn it (target=arg1,
// flag 1 then 0, template D_00256790 then D_00256820, descriptor
// D_0081F8F0, 0).

extern void func_001CFB50(void *p, int a, int b, float f0, float f1, float f2, float f3, float f4);
extern void func_001CFBE0(int a, int b, void *c, void *d, int e);

extern int *D_00275C34;
extern unsigned char D_00256790[16];
extern unsigned char D_00256820[16];
extern unsigned char D_0081F8F0[16];

void func_001EC270(int arg0, int arg1) {
    int r;

    r = D_00275C34[1];
    {
        float f = (float)((r >> 0x10) & 0xFFFF) / 65535.0f;
        D_00275C34[1] = r * 0x25 + 0xB;
        func_001CFB50(D_0081F8F0, 0, arg0, *(float *)((char *)D_00275C34 + 0x54),
                      f + 1e-4f, 1.0f, 1e-6f, 5.0f);
    }
    func_001CFBE0(arg1, 1, D_00256790, D_0081F8F0, 0);

    r = D_00275C34[1];
    {
        float f = (float)((r >> 0x10) & 0xFFFF) / 65535.0f;
        D_00275C34[1] = r * 0x25 + 0xB;
        func_001CFB50(D_0081F8F0, 0, arg0, *(float *)((char *)D_00275C34 + 0x54),
                      f + 1e-4f, 1.0f, 1e-6f, 5.0f);
    }
    func_001CFBE0(arg1, 0, D_00256820, D_0081F8F0, 0);
}
