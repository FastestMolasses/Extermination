// NEARMISS func_001EF1C0  (vram 0x001EF1C0, 0x350 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 96.57% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FP register-coloring permutation in the func_001CFB50 call setup. Body/structure/init-block 100% recovered. The three FP constant args (1.0f, 1e-6f, 10.0f) color to different FP registers and load in a different order than the target (target fa1f/ft4/fa1, mine fa2/fa3/fa4); the tail-clamp float t...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// NEARMISS (mwcc233 = 96.57%, 991202 = 88.03%): body, structure, and the
// absolute in-order init block fully recovered. Sole residual is an FP
// register-coloring / paired-single allocation permutation in the
// func_001CFB50 call setup -- the three FP constant args (1.0f, 1e-6f, 10.0f)
// color to fa2/fa3/fa4 here vs the target's fa1f/ft4/fa1 and load in a
// different order; the tail-clamp temp colors differently too. Permuter
// territory; 2.3.3 does not fix it. Same wall as sibling NEARMISS
// func_001ED450/func_001ED7A0/func_001EEBA0 (shared descriptor template).
//
// Periodic particle/effect re-seed (sibling of func_001ED450). Rewrites three
// 8-word (0x20) global descriptor blocks at D_00257530, D_002575C0, D_00257650
// to their constant default vectors (24/24/48/0... and 96/96/192/96/64/64/160/0
// twice). Then for each of three sub-emitters: advances the global LCG at
// D_00275C34[1] (state = state*0x25 + 0xB), derives a normalized random scalar
// f = ((state>>16 & 0xFFFF)/65535) + 0.0001, primes the shared param buffer
// D_0081F8F0 via func_001CFB50(buf, 0, arg0, D_00275C34+0x54 timer, f, 1.0,
// 9.99e-7, 10.0), and submits the emitter for descriptor table D_00257510 /
// D_002575A0 / D_00257630 via func_001CFBE0(arg1, 1, table, buf, 1). Finally
// eases the global fade D_00275C34+8 toward 0.02 by a tenth each call and clamps
// it to a 0.02 floor.
//
// -sdatathreshold 4: D_00275C34 (gp-range pointer) stays gp-relative (matches
// CW); the init globals are declared volatile float[2] so they exceed the sdata
// threshold, stay absolute (per-word hi/lo relocs), and emit in address order.
extern int *D_00275C34;
extern int D_00257510[2];
extern int D_002575A0[2];
extern int D_00257630[2];
extern int D_0081F8F0[2];
extern volatile float D_00257530[2], D_00257534[2], D_00257538[2], D_0025753C[2], D_00257540[2], D_00257544[2], D_00257548[2], D_0025754C[2];
extern volatile float D_002575C0[2], D_002575C4[2], D_002575C8[2], D_002575CC[2], D_002575D0[2], D_002575D4[2], D_002575D8[2], D_002575DC[2];
extern volatile float D_00257650[2], D_00257654[2], D_00257658[2], D_0025765C[2], D_00257660[2], D_00257664[2], D_00257668[2], D_0025766C[2];

void func_001CFB50(void *p, int b, int c, float f0, float f1, float f2, float f3, float f4);
void func_001CFBE0(int a, int b, void *c, void *d, int e);

void func_001EF1C0(int arg0, int arg1) {
    int r;
    float f;
    float v;

    D_00257530[0] = 24.0f; D_00257534[0] = 24.0f; D_00257538[0] = 48.0f;
    D_0025753C[0] = 0.0f; D_00257540[0] = 0.0f; D_00257544[0] = 0.0f; D_00257548[0] = 0.0f; D_0025754C[0] = 0.0f;
    D_002575C0[0] = 96.0f; D_002575C4[0] = 96.0f; D_002575C8[0] = 192.0f; D_002575CC[0] = 96.0f; D_002575D0[0] = 64.0f; D_002575D4[0] = 64.0f; D_002575D8[0] = 160.0f; D_002575DC[0] = 0.0f;
    D_00257650[0] = 96.0f; D_00257654[0] = 96.0f; D_00257658[0] = 192.0f; D_0025765C[0] = 96.0f; D_00257660[0] = 64.0f; D_00257664[0] = 64.0f; D_00257668[0] = 160.0f; D_0025766C[0] = 0.0f;

    r = D_00275C34[1];
    f = (float)((r >> 0x10) & 0xFFFF);
    f /= 65535.0f;
    D_00275C34[1] = r * 0x25 + 0xB;
    f += 0.0001f;
    func_001CFB50(D_0081F8F0, 0, arg0, *(float *)((char *)D_00275C34 + 0x54), f, 1.0f, 9.999999974752427e-07f, 10.0f);
    func_001CFBE0(arg1, 1, D_00257510, D_0081F8F0, 1);

    r = D_00275C34[1];
    f = (float)((r >> 0x10) & 0xFFFF);
    f /= 65535.0f;
    D_00275C34[1] = r * 0x25 + 0xB;
    f += 0.0001f;
    func_001CFB50(D_0081F8F0, 0, arg0, *(float *)((char *)D_00275C34 + 0x54), f, 1.0f, 9.999999974752427e-07f, 10.0f);
    func_001CFBE0(arg1, 1, D_002575A0, D_0081F8F0, 1);

    r = D_00275C34[1];
    f = (float)((r >> 0x10) & 0xFFFF);
    f /= 65535.0f;
    D_00275C34[1] = r * 0x25 + 0xB;
    f += 0.0001f;
    func_001CFB50(D_0081F8F0, 0, arg0, *(float *)((char *)D_00275C34 + 0x54), f, 1.0f, 9.999999974752427e-07f, 10.0f);
    func_001CFBE0(arg1, 1, D_00257630, D_0081F8F0, 1);

    v = *(float *)((char *)D_00275C34 + 8);
    v += (0.02f - v) / 10.0f;
    *(float *)((char *)D_00275C34 + 8) = v;
    v = *(float *)((char *)D_00275C34 + 8);
    if (v < 0.02f) { v = 0.02f; }
    *(float *)((char *)D_00275C34 + 8) = v;
}
