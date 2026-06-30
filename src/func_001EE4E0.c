// NEARMISS func_001EE4E0  (vram 0x001EE4E0, 0x350 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 85.78% via mwcc 2.3 (mwcps2-2.3-991202) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// see above
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc
// CFLAGS: -O4,p -sdatathreshold 4

//
// SEMANTICS: Effect/animation setup, sibling of func_001EDE40. Initializes a fixed table of float
// parameters (three records at D_002576C0 / D_00257750 / D_002577E0; written slots at field offsets
// 0x20..0x3C of each record), then runs three event-spawn passes and one easing update:
//   For each record R in {D_002576C0, D_00257750, D_002577E0}:
//     r = state->index4;                              // *(int*)(D_00275C34 + 4)
//     phase = (float)((r >> 16) & 0xFFFF) / 65535.0f + 1e-4f;
//     state->index4 = r * 37 + 11;                    // LCG-style advance (0x25 = 37, 0xB = 11)
//     func_001CFB50(D_0081F8F0, 0, arg0, state->f54, phase, 1.0f, 1e-6f, 15.0f);
//   Then: pass1 func_001CFBE0(arg1, 1, D_002576C0, D_0081F8F0, 1);
//         pass2 func_001CFBE0(arg1, 6, D_00257750, D_0081F8F0, 1);   // note a1=6
//         pass3 func_001CFBE0(arg1, 1, D_002577E0, D_0081F8F0, 1);
//   Easing of state->f8 toward 0.02 with a /10 step, clamped to a floor of 0.02:
//     state->f8 += (0.02f - state->f8) / 10.0f;  if (state->f8 < 0.02f) state->f8 = 0.02f;
// The D_002577xx/D_002578xx table slots are declared volatile float[3] so mwcc keeps absolute
// (%hi/%lo) addressing and source store order; the small D_00275C34 pointer stays gp-rel
// (-sdatathreshold 4); the [0] index keeps each relocation pointed at its own splat symbol, addend 0.

extern void func_001CFB50(void *p, int a1, int a2, float f12, float f13, float f14, float f15, float f16);
extern void func_001CFBE0(int a0, int a1, void *a2, void *a3, int a4);

extern volatile float D_002576E0[3];
extern volatile float D_002576E4[3];
extern volatile float D_002576E8[3];
extern volatile float D_002576EC[3];
extern volatile float D_002576F0[3];
extern volatile float D_002576F4[3];
extern volatile float D_002576F8[3];
extern volatile float D_002576FC[3];
extern volatile float D_00257770[3];
extern volatile float D_00257774[3];
extern volatile float D_00257778[3];
extern volatile float D_0025777C[3];
extern volatile float D_00257780[3];
extern volatile float D_00257784[3];
extern volatile float D_00257788[3];
extern volatile float D_0025778C[3];
extern volatile float D_00257800[3];
extern volatile float D_00257804[3];
extern volatile float D_00257808[3];
extern volatile float D_0025780C[3];
extern volatile float D_00257810[3];
extern volatile float D_00257814[3];
extern volatile float D_00257818[3];
extern volatile float D_0025781C[3];
extern char D_002576C0[256];
extern char D_00257750[256];
extern char D_002577E0[256];
extern char *D_00275C34;
extern char D_0081F8F0[256];

void func_001EE4E0(int arg0, int arg1) {
    int r;
    float f;
    float t;
    float v;

    D_002576E0[0] = 12.0f;
    D_002576E4[0] = 12.0f;
    D_002576E8[0] = 16.0f;
    D_002576EC[0] = 0.0f;
    D_002576F0[0] = 0.0f;
    D_002576F4[0] = 0.0f;
    D_002576F8[0] = 0.0f;
    D_002576FC[0] = 0.0f;
    D_00257770[0] = 32.0f;
    D_00257774[0] = 32.0f;
    D_00257778[0] = 64.0f;
    D_0025777C[0] = 96.0f;
    D_00257780[0] = 16.0f;
    D_00257784[0] = 16.0f;
    D_00257788[0] = 64.0f;
    D_0025778C[0] = 0.0f;
    D_00257800[0] = 32.0f;
    D_00257804[0] = 32.0f;
    D_00257808[0] = 64.0f;
    D_0025780C[0] = 96.0f;
    D_00257810[0] = 16.0f;
    D_00257814[0] = 16.0f;
    D_00257818[0] = 64.0f;
    D_0025781C[0] = 0.0f;

    r = *(int *)(D_00275C34 + 4);
    f = (float)((r >> 0x10) & 0xFFFF) / 65535.0f + 9.999999747378752e-05f;
    *(int *)(D_00275C34 + 4) = r * 0x25 + 0xB;
    func_001CFB50(D_0081F8F0, 0, arg0, *(float *)(D_00275C34 + 0x54),
                  f, 1.0f, 9.999999974752427e-07f, 15.0f);
    func_001CFBE0(arg1, 1, D_002576C0, D_0081F8F0, 1);

    r = *(int *)(D_00275C34 + 4);
    f = (float)((r >> 0x10) & 0xFFFF) / 65535.0f + 9.999999747378752e-05f;
    *(int *)(D_00275C34 + 4) = r * 0x25 + 0xB;
    func_001CFB50(D_0081F8F0, 0, arg0, *(float *)(D_00275C34 + 0x54),
                  f, 1.0f, 9.999999974752427e-07f, 15.0f);
    func_001CFBE0(arg1, 6, D_00257750, D_0081F8F0, 1);

    r = *(int *)(D_00275C34 + 4);
    f = (float)((r >> 0x10) & 0xFFFF) / 65535.0f + 9.999999747378752e-05f;
    *(int *)(D_00275C34 + 4) = r * 0x25 + 0xB;
    func_001CFB50(D_0081F8F0, 0, arg0, *(float *)(D_00275C34 + 0x54),
                  f, 1.0f, 9.999999974752427e-07f, 15.0f);
    func_001CFBE0(arg1, 1, D_002577E0, D_0081F8F0, 1);

    t = *(float *)(D_00275C34 + 8);
    *(float *)(D_00275C34 + 8) = t + (0.02f - t) / 10.0f;
    v = *(float *)(D_00275C34 + 8);
    if (v < 0.02f) {
        v = 0.02f;
    }
    *(float *)(D_00275C34 + 8) = v;
}
