// NEARMISS func_001EE190  (vram 0x001EE190, 0x350 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 83.68% via mwcc 2.3 (mwcps2-2.3-991202) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Body and structure fully recovered (objdiff 83.68% via mwcc 991202 -O4,p -sdatathreshold 4; 2.3.3 LOWER at 81.91%). Entire 24-store global-init table matches byte-for-byte (all values/symbols identical), gp-rel D_00275C34 matches, easing tail matches. Sole residual is the proven scheduling/FP-col...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc
// CFLAGS: -O4,p -sdatathreshold 4

//
// SEMANTICS: Effect/animation setup, sibling of func_001EDE40. Initializes a fixed table of float
// parameters (three records at D_00257360 / D_002573F0 / D_00257480; written slots at field offsets
// 0x20..0x3C of each record), then runs three identical event-spawn passes and one easing update:
//   For each record R in {D_00257360, D_002573F0, D_00257480}:
//     r = state->index4;                              // *(int*)(D_00275C34 + 4)
//     phase = (float)((r >> 16) & 0xFFFF) / 65535.0f + 1e-4f;
//     state->index4 = r * 37 + 11;                    // LCG-style advance (0x25 = 37, 0xB = 11)
//     func_001CFB50(D_0081F8F0, 0, arg0, state->f54, phase, 1.0f, 1e-6f, 10.0f);
//     func_001CFBE0(arg1, 1, R, D_0081F8F0, 0);       // fire one event on the built object
//   Easing of state->f8 toward 0.02 with a /10 step, clamped to a floor of 0.02:
//     state->f8 += (0.02f - state->f8) / 10.0f;  if (state->f8 < 0.02f) state->f8 = 0.02f;
// The D_002574xx table slots are declared volatile float[3] so mwcc keeps absolute (%hi/%lo)
// addressing and source store order; the small D_00275C34 pointer stays gp-rel (-sdatathreshold 4);
// the [0] index keeps each relocation pointed at its own splat symbol with addend 0.

extern void func_001CFB50(void *p, int a1, int a2, float f12, float f13, float f14, float f15, float f16);
extern void func_001CFBE0(int a0, int a1, void *a2, void *a3, int a4);

extern volatile float D_00257380[3];
extern volatile float D_00257384[3];
extern volatile float D_00257388[3];
extern volatile float D_0025738C[3];
extern volatile float D_00257390[3];
extern volatile float D_00257394[3];
extern volatile float D_00257398[3];
extern volatile float D_0025739C[3];
extern volatile float D_00257410[3];
extern volatile float D_00257414[3];
extern volatile float D_00257418[3];
extern volatile float D_0025741C[3];
extern volatile float D_00257420[3];
extern volatile float D_00257424[3];
extern volatile float D_00257428[3];
extern volatile float D_0025742C[3];
extern volatile float D_002574A0[3];
extern volatile float D_002574A4[3];
extern volatile float D_002574A8[3];
extern volatile float D_002574AC[3];
extern volatile float D_002574B0[3];
extern volatile float D_002574B4[3];
extern volatile float D_002574B8[3];
extern volatile float D_002574BC[3];
extern char D_00257360[256];
extern char D_002573F0[256];
extern char D_00257480[256];
extern char *D_00275C34;
extern char D_0081F8F0[256];

void func_001EE190(int arg0, int arg1) {
    int r;
    float f;
    float t;
    float v;

    D_00257380[0] = 12.0f;
    D_00257384[0] = 12.0f;
    D_00257388[0] = 16.0f;
    D_0025738C[0] = 0.0f;
    D_00257390[0] = 0.0f;
    D_00257394[0] = 0.0f;
    D_00257398[0] = 0.0f;
    D_0025739C[0] = 0.0f;
    D_00257410[0] = 32.0f;
    D_00257414[0] = 32.0f;
    D_00257418[0] = 64.0f;
    D_0025741C[0] = 96.0f;
    D_00257420[0] = 16.0f;
    D_00257424[0] = 16.0f;
    D_00257428[0] = 64.0f;
    D_0025742C[0] = 0.0f;
    D_002574A0[0] = 32.0f;
    D_002574A4[0] = 32.0f;
    D_002574A8[0] = 64.0f;
    D_002574AC[0] = 96.0f;
    D_002574B0[0] = 16.0f;
    D_002574B4[0] = 16.0f;
    D_002574B8[0] = 64.0f;
    D_002574BC[0] = 0.0f;

    r = *(int *)(D_00275C34 + 4);
    f = (float)((r >> 0x10) & 0xFFFF) / 65535.0f + 9.999999747378752e-05f;
    *(int *)(D_00275C34 + 4) = r * 0x25 + 0xB;
    func_001CFB50(D_0081F8F0, 0, arg0, *(float *)(D_00275C34 + 0x54),
                  f, 1.0f, 9.999999974752427e-07f, 10.0f);
    func_001CFBE0(arg1, 1, D_00257360, D_0081F8F0, 0);

    r = *(int *)(D_00275C34 + 4);
    f = (float)((r >> 0x10) & 0xFFFF) / 65535.0f + 9.999999747378752e-05f;
    *(int *)(D_00275C34 + 4) = r * 0x25 + 0xB;
    func_001CFB50(D_0081F8F0, 0, arg0, *(float *)(D_00275C34 + 0x54),
                  f, 1.0f, 9.999999974752427e-07f, 10.0f);
    func_001CFBE0(arg1, 1, D_002573F0, D_0081F8F0, 0);

    r = *(int *)(D_00275C34 + 4);
    f = (float)((r >> 0x10) & 0xFFFF) / 65535.0f + 9.999999747378752e-05f;
    *(int *)(D_00275C34 + 4) = r * 0x25 + 0xB;
    func_001CFB50(D_0081F8F0, 0, arg0, *(float *)(D_00275C34 + 0x54),
                  f, 1.0f, 9.999999974752427e-07f, 10.0f);
    func_001CFBE0(arg1, 1, D_00257480, D_0081F8F0, 0);

    t = *(float *)(D_00275C34 + 8);
    *(float *)(D_00275C34 + 8) = t + (0.02f - t) / 10.0f;
    v = *(float *)(D_00275C34 + 8);
    if (v < 0.02f) {
        v = 0.02f;
    }
    *(float *)(D_00275C34 + 8) = v;
}
