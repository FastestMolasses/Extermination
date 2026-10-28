// NEARMISS func_001EE830  (vram 0x001EE830, 0x364 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 96.11% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Remaining differences: the order in which the three constant call arguments (1.0f, 1e-6f, 15.0f) of func_001CFB50 are materialized in the first and third calls (the target sets up 1e-6f/15.0f before 1.0f in call 1 and 15.0f early in call 3; call 2 matches) -- the same residual as siblings func_001EF1C0/func_001EF780. The init block (volatile float[8] arrays, indexed) and the easing tail (in-place update through a field pointer) now match exactly. Not fixed by int-staged constants (idiom-24), constant locals in any declaration order, double literals, a timer local, a void* callee parameter, or compiler mwcc24.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// NEARMISS (mwcc233 = 96.11%): body, structure, init block and easing tail
// fully recovered; the sole residual is the func_001CFB50 constant-argument
// setup order shared by the whole sibling family.
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
// volatile float[8] to stay absolute (hi/lo) and emit in address order.
extern int *D_00275C34;
extern int D_002576C0[2];
extern int D_00257750[2];
extern int D_002577E0[2];
extern int D_0081F8F0[2];
extern volatile float D_002576E0[8];
extern volatile float D_00257770[8];
extern volatile float D_00257800[8];

void func_001CFB50(void *p, int b, int c, float f0, float f1, float f2, float f3, float f4);
void func_001CFBE0(int a, int b, void *c, void *d, int e);

void func_001EE830(int arg0, int arg1) {
    int r;
    float f;
    float v;

    D_002576E0[0] = 12.0f; D_002576E0[1] = 12.0f; D_002576E0[2] = 16.0f; D_002576E0[3] = 0.0f; D_002576E0[4] = 0.0f; D_002576E0[5] = 0.0f; D_002576E0[6] = 0.0f; D_002576E0[7] = 0.0f;
    D_00257770[0] = 32.0f; D_00257770[1] = 32.0f; D_00257770[2] = 64.0f; D_00257770[3] = 96.0f; D_00257770[4] = 16.0f; D_00257770[5] = 16.0f; D_00257770[6] = 64.0f; D_00257770[7] = 0.0f;
    D_00257800[0] = 32.0f; D_00257800[1] = 32.0f; D_00257800[2] = 64.0f; D_00257800[3] = 96.0f; D_00257800[4] = 16.0f; D_00257800[5] = 16.0f; D_00257800[6] = 64.0f; D_00257800[7] = 0.0f;

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

    *(float *)((char *)arg0 + 0x38) -= 2.0f;

    {
        float *p = (float *)((char *)D_00275C34 + 8);
        *p += (0.02f - *p) / 10.0f;
    }
    v = *(float *)((char *)D_00275C34 + 8);
    if (v < 0.02f) { v = 0.02f; }
    *(float *)((char *)D_00275C34 + 8) = v;
}
