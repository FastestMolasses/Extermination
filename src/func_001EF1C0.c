// NEARMISS func_001EF1C0  (vram 0x001EF1C0, 0x350 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 96.80% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Remaining differences: the order in which the three constant call arguments (1.0f, 1e-6f, 10.0f) of func_001CFB50 are materialized in the first and third calls (the target rotates them: 1e-6f/10.0f before 1.0f in call 1, 10.0f first through a separate integer register in call 3; call 2 already matches), which also costs one unfilled slot. The easing tail now matches exactly when written as an in-place update through a field pointer (*p += (0.02f - *p) / 10.0f). Not fixed by constant locals, int-staged constants, double literals, statement orders, a 3-iteration loop, int-bit init stores, callee pointer prototype, or compilers 991202/2.4.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// NEARMISS (mwcc233 = 96.80%, 991202 = 88.03%): body, structure, and the
// absolute in-order init block fully recovered. Sole residual is an FP
// register-coloring / paired-single allocation permutation in the
// func_001CFB50 call setup -- the three FP constant args (1.0f, 1e-6f, 10.0f)
// materialize in a different order than the target in calls 1 and 3. Permuter
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
// CW); the init globals are declared volatile float[8] so they exceed the sdata
// threshold, stay absolute (per-word hi/lo relocs), and emit in address order.
extern int *D_00275C34;
extern int D_00257510[2];
extern int D_002575A0[2];
extern int D_00257630[2];
extern int D_0081F8F0[2];
extern volatile float D_00257530[8];
extern volatile float D_002575C0[8];
extern volatile float D_00257650[8];

void func_001CFB50(void *p, int b, int c, float f0, float f1, float f2, float f3, float f4);
void func_001CFBE0(int a, int b, void *c, void *d, int e);

void func_001EF1C0(int arg0, int arg1) {
    int r;
    float f;
    float v;

    D_00257530[0] = 24.0f; D_00257530[1] = 24.0f; D_00257530[2] = 48.0f; D_00257530[3] = 0.0f; D_00257530[4] = 0.0f; D_00257530[5] = 0.0f; D_00257530[6] = 0.0f; D_00257530[7] = 0.0f;
    D_002575C0[0] = 96.0f; D_002575C0[1] = 96.0f; D_002575C0[2] = 192.0f; D_002575C0[3] = 96.0f; D_002575C0[4] = 64.0f; D_002575C0[5] = 64.0f; D_002575C0[6] = 160.0f; D_002575C0[7] = 0.0f;
    D_00257650[0] = 96.0f; D_00257650[1] = 96.0f; D_00257650[2] = 192.0f; D_00257650[3] = 96.0f; D_00257650[4] = 64.0f; D_00257650[5] = 64.0f; D_00257650[6] = 160.0f; D_00257650[7] = 0.0f;

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

    {
        float *p = (float *)((char *)D_00275C34 + 8);
        *p += (0.02f - *p) / 10.0f;
    }
    v = *(float *)((char *)D_00275C34 + 8);
    if (v < 0.02f) { v = 0.02f; }
    *(float *)((char *)D_00275C34 + 8) = v;
}
