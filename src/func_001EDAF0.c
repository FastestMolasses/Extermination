// NEARMISS func_001EDAF0  (vram 0x001EDAF0, 0x34C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 82.52% via mwcc 2.3 (mwcps2-2.3-991202) (-O4,p). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FP-constant emit-order + float-div/int-mul instruction scheduling + FP register-coloring permutation. Logic and structure fully recovered: the 24-store global-init table and the easing tail match exactly; the residual is in the three identical func_001CFB50 call setups where the CW target emits t...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc
// CFLAGS: -O4,p

// NEARMISS func_001EDAF0  (vram 0x001EDAF0, 0x34C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 82.52% via mwcc 991202 (-O4,p) / 81.82% via mwcc 2.3.3. The LOGIC and STRUCTURE are
// fully recovered and identical to the target; the entire global-init store table (24 stores) and
// the easing tail match exactly. The residual diff is a genuine compiler artifact no source change
// fixes: FP-constant emit-order + float-div/int-mul instruction scheduling + FP register coloring
// permutation. Same wall class as the verified-NEARMISS sibling func_001EBE10 (identical call shape):
// the CW target emits the f15(1e-6f)/f16(10.0f) constants and schedules the cvt.s.w/div.s of the
// per-call float relative to the integer index math (r*0x25+0xB store) in an order mwcc (991202 and
// 2.3.3) will not reproduce, and colors the FP temps (fv1f/ft0) differently. Not the clean-store nop,
// so 2.3.3 does not help; this is register/scheduling-permuter territory.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT from this C
// (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit / excluded from matched_code.
//
//
// SEMANTICS: Effect/animation setup. Initializes a fixed table of float parameters (three 0x90-byte
// records starting at D_00257510 / D_002575A0 / D_00257630; the written slots are at field offsets
// 0x20..0x3C of each record), then runs three identical event-spawn passes and one easing update:
//   For each of the three records R in {D_00257510, D_002575A0, D_00257630}:
//     r = state->index4;                              // *(int*)(D_00275C34 + 4)
//     phase = (float)((r >> 16) & 0xFFFF) / 65535.0f + 1e-4f;
//     state->index4 = r * 37 + 11;                    // LCG-style advance (0x25 = 37, 0xB = 11)
//     func_001CFB50(D_0081F8F0, 0, arg0, state->f54, phase, 1.0f, 1e-6f, 10.0f);
//     func_001CFBE0(arg1, 1, R, D_0081F8F0, 0);       // fire one event on the built object
//   Easing of state->f8 toward 0.02 with a /10 step, clamped to a floor of 0.02:
//     state->f8 += (0.02f - state->f8) / 10.0f;  if (state->f8 < 0.02f) state->f8 = 0.02f;
// 0x358637BD is exactly the float 9.999999974752427e-07 (1e-6f); 0x38D1B717 is 9.999999747e-05 (1e-4f).
// The D_002575xx table slots are declared volatile float[3] so mwcc keeps absolute (%hi/%lo) addressing
// and the source store order (matching the target) while the small D_00275C34 pointer stays gp-rel;
// the [0] index keeps each relocation pointed at its own splat symbol with addend 0.

extern void func_001CFB50(void *p, int a1, int a2, float f12, float f13, float f14, float f15, float f16);
extern void func_001CFBE0(int a0, int a1, void *a2, void *a3, int a4);

extern volatile float D_00257530[3];
extern volatile float D_00257534[3];
extern volatile float D_00257538[3];
extern volatile float D_0025753C[3];
extern volatile float D_00257540[3];
extern volatile float D_00257544[3];
extern volatile float D_00257548[3];
extern volatile float D_0025754C[3];
extern volatile float D_002575C0[3];
extern volatile float D_002575C4[3];
extern volatile float D_002575C8[3];
extern volatile float D_002575CC[3];
extern volatile float D_002575D0[3];
extern volatile float D_002575D4[3];
extern volatile float D_002575D8[3];
extern volatile float D_002575DC[3];
extern volatile float D_00257650[3];
extern volatile float D_00257654[3];
extern volatile float D_00257658[3];
extern volatile float D_0025765C[3];
extern volatile float D_00257660[3];
extern volatile float D_00257664[3];
extern volatile float D_00257668[3];
extern volatile float D_0025766C[3];
extern char D_00257510[256];
extern char D_002575A0[256];
extern char D_00257630[256];
extern char *D_00275C34;
extern char D_0081F8F0[256];

void func_001EDAF0(int arg0, int arg1) {
    int r;
    float f;
    float t;
    float v;

    D_00257530[0] = 16.0f;
    D_00257534[0] = 12.0f;
    D_00257538[0] = 16.0f;
    D_0025753C[0] = 0.0f;
    D_00257540[0] = 0.0f;
    D_00257544[0] = 0.0f;
    D_00257548[0] = 0.0f;
    D_0025754C[0] = 0.0f;
    D_002575C0[0] = 128.0f;
    D_002575C4[0] = 0.0f;
    D_002575C8[0] = 128.0f;
    D_002575CC[0] = 96.0f;
    D_002575D0[0] = 96.0f;
    D_002575D4[0] = 0.0f;
    D_002575D8[0] = 96.0f;
    D_002575DC[0] = 0.0f;
    D_00257650[0] = 128.0f;
    D_00257654[0] = 0.0f;
    D_00257658[0] = 128.0f;
    D_0025765C[0] = 96.0f;
    D_00257660[0] = 96.0f;
    D_00257664[0] = 0.0f;
    D_00257668[0] = 96.0f;
    D_0025766C[0] = 0.0f;

    r = *(int *)(D_00275C34 + 4);
    f = (float)((r >> 0x10) & 0xFFFF) / 65535.0f + 9.999999747378752e-05f;
    *(int *)(D_00275C34 + 4) = r * 0x25 + 0xB;
    func_001CFB50(D_0081F8F0, 0, arg0, *(float *)(D_00275C34 + 0x54),
                  f, 1.0f, 9.999999974752427e-07f, 10.0f);
    func_001CFBE0(arg1, 1, D_00257510, D_0081F8F0, 0);

    r = *(int *)(D_00275C34 + 4);
    f = (float)((r >> 0x10) & 0xFFFF) / 65535.0f + 9.999999747378752e-05f;
    *(int *)(D_00275C34 + 4) = r * 0x25 + 0xB;
    func_001CFB50(D_0081F8F0, 0, arg0, *(float *)(D_00275C34 + 0x54),
                  f, 1.0f, 9.999999974752427e-07f, 10.0f);
    func_001CFBE0(arg1, 1, D_002575A0, D_0081F8F0, 0);

    r = *(int *)(D_00275C34 + 4);
    f = (float)((r >> 0x10) & 0xFFFF) / 65535.0f + 9.999999747378752e-05f;
    *(int *)(D_00275C34 + 4) = r * 0x25 + 0xB;
    func_001CFB50(D_0081F8F0, 0, arg0, *(float *)(D_00275C34 + 0x54),
                  f, 1.0f, 9.999999974752427e-07f, 10.0f);
    func_001CFBE0(arg1, 1, D_00257630, D_0081F8F0, 0);

    t = *(float *)(D_00275C34 + 8);
    *(float *)(D_00275C34 + 8) = t + (0.02f - t) / 10.0f;
    v = *(float *)(D_00275C34 + 8);
    if (v < 0.02f) {
        v = 0.02f;
    }
    *(float *)(D_00275C34 + 8) = v;
}
