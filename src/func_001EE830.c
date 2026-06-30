// NEARMISS func_001EE830  (vram 0x001EE830, 0x364 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 95.88% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FP register-coloring permutation in the func_001CFB50 call setup, identical to sibling func_001EF1C0. Body/structure/init-block 100% recovered, including the extra pre-easing step *(arg0+0x38) -= 2.0f. The three FP constants (1.0f, 1e-6f, 15.0f) color to fa2/fa3/fa4 here vs target fa1f/ft4/fa1 an...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// NEARMISS (mwcc233 = 95.88%, 991202 = 87.49%): body, structure, and the
// absolute in-order init block fully recovered. Sole residual is the same
// FP register-coloring permutation as sibling func_001EF1C0 -- the func_001CFB50
// call's FP constant args (1.0f, 1e-6f, 15.0f) color to fa2/fa3/fa4 here vs the
// target's fa1f/ft4/fa1 and load in a different order; the tail-clamp temp colors
// differently. Permuter territory; 2.3.3 does not fix it.
//
// Periodic particle/effect re-seed (sibling of func_001EF1C0 / func_001ED450).
// Rewrites three 8-word (0x20) global descriptor blocks at D_002576E0,
// D_00257770, D_00257800 to their constant default vectors (12/12/16/0... and
// 32/32/64/96/16/16/64/0 twice). Then for each of three sub-emitters: advances
// the global LCG at D_00275C34[1] (state = state*0x25 + 0xB), derives a
// normalized random scalar f = ((state>>16 & 0xFFFF)/65535) + 0.0001, primes
// the shared param buffer D_0081F8F0 via func_001CFB50(buf, 0, arg0,
// D_00275C34+0x54 timer, f, 1.0, 9.99e-7, 15.0), and submits the emitter for
// descriptor table D_002576C0 / D_00257750 / D_002577E0 via
// func_001CFBE0(arg1, 1, table, buf, 1). Then decrements the caller's
// *(arg0+0x38) float field by 2.0, and finally eases the global fade
// D_00275C34+8 toward 0.02 by a tenth each call and clamps it to a 0.02 floor.
//
// -sdatathreshold 4: D_00275C34 stays gp-relative; init globals declared
// volatile float[2] to stay absolute (hi/lo) and emit in address order.
extern int *D_00275C34;
extern int D_002576C0[2];
extern int D_00257750[2];
extern int D_002577E0[2];
extern int D_0081F8F0[2];
extern volatile float D_002576E0[2], D_002576E4[2], D_002576E8[2], D_002576EC[2], D_002576F0[2], D_002576F4[2], D_002576F8[2], D_002576FC[2];
extern volatile float D_00257770[2], D_00257774[2], D_00257778[2], D_0025777C[2], D_00257780[2], D_00257784[2], D_00257788[2], D_0025778C[2];
extern volatile float D_00257800[2], D_00257804[2], D_00257808[2], D_0025780C[2], D_00257810[2], D_00257814[2], D_00257818[2], D_0025781C[2];

void func_001CFB50(void *p, int b, int c, float f0, float f1, float f2, float f3, float f4);
void func_001CFBE0(int a, int b, void *c, void *d, int e);

void func_001EE830(int arg0, int arg1) {
    int r;
    float f;
    float v;

    D_002576E0[0] = 12.0f; D_002576E4[0] = 12.0f; D_002576E8[0] = 16.0f;
    D_002576EC[0] = 0.0f; D_002576F0[0] = 0.0f; D_002576F4[0] = 0.0f; D_002576F8[0] = 0.0f; D_002576FC[0] = 0.0f;
    D_00257770[0] = 32.0f; D_00257774[0] = 32.0f; D_00257778[0] = 64.0f; D_0025777C[0] = 96.0f; D_00257780[0] = 16.0f; D_00257784[0] = 16.0f; D_00257788[0] = 64.0f; D_0025778C[0] = 0.0f;
    D_00257800[0] = 32.0f; D_00257804[0] = 32.0f; D_00257808[0] = 64.0f; D_0025780C[0] = 96.0f; D_00257810[0] = 16.0f; D_00257814[0] = 16.0f; D_00257818[0] = 64.0f; D_0025781C[0] = 0.0f;

    r = D_00275C34[1];
    f = (float)((r >> 0x10) & 0xFFFF);
    f /= 65535.0f;
    D_00275C34[1] = r * 0x25 + 0xB;
    f += 0.0001f;
    func_001CFB50(D_0081F8F0, 0, arg0, *(float *)((char *)D_00275C34 + 0x54), f, 1.0f, 9.999999974752427e-07f, 15.0f);
    func_001CFBE0(arg1, 1, D_002576C0, D_0081F8F0, 1);

    r = D_00275C34[1];
    f = (float)((r >> 0x10) & 0xFFFF);
    f /= 65535.0f;
    D_00275C34[1] = r * 0x25 + 0xB;
    f += 0.0001f;
    func_001CFB50(D_0081F8F0, 0, arg0, *(float *)((char *)D_00275C34 + 0x54), f, 1.0f, 9.999999974752427e-07f, 15.0f);
    func_001CFBE0(arg1, 1, D_00257750, D_0081F8F0, 1);

    r = D_00275C34[1];
    f = (float)((r >> 0x10) & 0xFFFF);
    f /= 65535.0f;
    D_00275C34[1] = r * 0x25 + 0xB;
    f += 0.0001f;
    func_001CFB50(D_0081F8F0, 0, arg0, *(float *)((char *)D_00275C34 + 0x54), f, 1.0f, 9.999999974752427e-07f, 15.0f);
    func_001CFBE0(arg1, 1, D_002577E0, D_0081F8F0, 1);

    *(float *)((char *)arg0 + 0x38) = *(float *)((char *)arg0 + 0x38) - 2.0f;

    v = *(float *)((char *)D_00275C34 + 8);
    v += (0.02f - v) / 10.0f;
    *(float *)((char *)D_00275C34 + 8) = v;
    v = *(float *)((char *)D_00275C34 + 8);
    if (v < 0.02f) { v = 0.02f; }
    *(float *)((char *)D_00275C34 + 8) = v;
}
