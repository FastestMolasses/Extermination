// NEARMISS func_001ED100  (vram 0x001ED100, 0x348 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 96.93% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Remaining differences: only the float-constant setup order (1.0f vs 10.0f) in the first and third func_001CFB50 calls; the middle call matches. This is the TU-neighbour constant-order class (MATCHING_GUIDE batch 27): compiled after func_001ECEF0 + func_001ECFB0 (TU_STATIC) the middle call matches, but the per-call order moves with any change in earlier compiled code (predecessor clamp spelling, local scoping) and no honest combination tried reproduced all three calls (best alternative 97.00% needs a non-canonical predecessor spelling). Block initializers are 8-float arrays, which keeps all %hi/%lo pairs identical.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
// TU_STATIC: func_001ECFB0 func_001ECEF0

//
// Periodic particle/effect re-seed, twin of func_001ED450/func_001ED7A0/func_001EEBA0.
// Rewrites three 8-float parameter blocks (D_00257380, D_00257410, D_002574A0 at +0x20
// of the 0x90-byte emitter records D_00257360/D_002573F0/D_00257480) to their defaults,
// then for each emitter advances the PRNG word D_00275C34[1] (r*0x25+0xB), derives
// f = ((r>>16)&0xFFFF)/65535 + 0.0001, calls func_001CFB50(D_0081F8F0, 0, arg0,
// <field 0x54>, f, 1.0f, 1e-6f, 10.0f) and func_001CFBE0(arg1, 1, record, D_0081F8F0, 1).
// Finally eases the float at D_00275C34+8 toward 0.02 by 1/10 and floors it at 0.02.
static void func_001ECFB0(int arg0, int arg1);  /* same-TU predecessor, see TU_STATIC */
#include "src/func_001ECFB0.c"
extern char D_00257360[0x90];
extern char D_002573F0[0x90];
extern char D_00257480[0x90];
extern volatile float D_00257380[8];
extern volatile float D_00257410[8];
extern volatile float D_002574A0[8];

void func_001ED100(int arg0, int arg1) {
    int r;
    float f;
    float v;

    D_00257380[0] = 16.0f; D_00257380[1] = 4.0f; D_00257380[2] = 4.0f; D_00257380[3] = 0.0f; D_00257380[4] = 0.0f; D_00257380[5] = 0.0f; D_00257380[6] = 0.0f; D_00257380[7] = 0.0f;
    D_00257410[0] = 96.0f; D_00257410[1] = 0.0f; D_00257410[2] = 0.0f; D_00257410[3] = 96.0f; D_00257410[4] = 96.0f; D_00257410[5] = 0.0f; D_00257410[6] = 0.0f; D_00257410[7] = 0.0f;
    D_002574A0[0] = 96.0f; D_002574A0[1] = 0.0f; D_002574A0[2] = 0.0f; D_002574A0[3] = 96.0f; D_002574A0[4] = 96.0f; D_002574A0[5] = 0.0f; D_002574A0[6] = 0.0f; D_002574A0[7] = 0.0f;

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
    func_001CFBE0(arg1, 1, D_002573F0, D_0081F8F0, 1);

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
        float *p = (float *)((char *)D_00275C34 + 8);
        *p += (0.02f - *p) / 10.0f;
    }
    v = *(float *)((char *)D_00275C34 + 8);
    if (v < 0.02f) {
        v = 0.02f;
    }
    *(float *)((char *)D_00275C34 + 8) = v;
}
