// NEARMISS func_001EE190  (vram 0x001EE190, 0x350 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 95.99% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Remaining differences: the order in which the three constant call arguments (1.0f, 1e-6f, 10.0f) of func_001CFB50 are materialized in the first and third calls (call 2 matches) -- the same residual as siblings func_001EF1C0/func_001EE830. It depends on earlier code in the same translation unit: a probe compiling the byte-matched func_001ECEF0 first (TU_STATIC) fixes call 3 (98.97%), so the real immediately-preceding chain (func_001ECFB0 .. func_001EDE40, all NEARMISS today) probably fixes it once matched. Not fixed by pointer-typed arg0/callee parameters, a char* state pointer, inline helpers, or compiler 991202.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// SEMANTICS: Effect/animation setup, sibling of func_001EDE40 and func_001EF1C0. Initializes three
// 8-float parameter blocks at D_00257380 / D_00257410 / D_002574A0 (field +0x20 of the records
// D_00257360 / D_002573F0 / D_00257480) to 12,12,16,0,0,0,0,0 and twice 32,32,64,96,16,16,64,0,
// then runs three identical event-spawn passes and one easing update:
//   For each record R in {D_00257360, D_002573F0, D_00257480}:
//     r = state[1];  f = ((r >> 16) & 0xFFFF) / 65535.0f + 1e-4f;  state[1] = r * 37 + 11;
//     func_001CFB50(D_0081F8F0, 0, arg0, state->f54, f, 1.0f, 1e-6f, 10.0f);
//     func_001CFBE0(arg1, 1, R, D_0081F8F0, 1);
//   Eases state->f8 toward 0.02 by a tenth of the gap, clamped to a 0.02 floor.
// D_00275C34 stays gp-relative (-sdatathreshold 4); the parameter blocks are volatile float[8]
// so they stay absolute and are stored in source order.
extern int *D_00275C34;
extern char D_00257360[256];
extern char D_002573F0[256];
extern char D_00257480[256];
extern char D_0081F8F0[256];
extern volatile float D_00257380[8];
extern volatile float D_00257410[8];
extern volatile float D_002574A0[8];

void func_001CFB50(void *p, int b, int c, float f0, float f1, float f2, float f3, float f4);
void func_001CFBE0(int a, int b, void *c, void *d, int e);

void func_001EE190(int arg0, int arg1) {
    int r;
    float f;
    float v;

    D_00257380[0] = 12.0f; D_00257380[1] = 12.0f; D_00257380[2] = 16.0f; D_00257380[3] = 0.0f; D_00257380[4] = 0.0f; D_00257380[5] = 0.0f; D_00257380[6] = 0.0f; D_00257380[7] = 0.0f;
    D_00257410[0] = 32.0f; D_00257410[1] = 32.0f; D_00257410[2] = 64.0f; D_00257410[3] = 96.0f; D_00257410[4] = 16.0f; D_00257410[5] = 16.0f; D_00257410[6] = 64.0f; D_00257410[7] = 0.0f;
    D_002574A0[0] = 32.0f; D_002574A0[1] = 32.0f; D_002574A0[2] = 64.0f; D_002574A0[3] = 96.0f; D_002574A0[4] = 16.0f; D_002574A0[5] = 16.0f; D_002574A0[6] = 64.0f; D_002574A0[7] = 0.0f;

    r = D_00275C34[1];
    f = (float)((r >> 0x10) & 0xFFFF);
    f /= 65535.0f;
    D_00275C34[1] = r * 0x25 + 0xB;
    f += 0.0001f;
    func_001CFB50(D_0081F8F0, 0, arg0, *(float *)((char *)D_00275C34 + 0x54), f, 1.0f, 9.999999974752427e-07f, 10.0f);
    func_001CFBE0(arg1, 1, D_00257360, D_0081F8F0, 1);

    r = D_00275C34[1];
    f = (float)((r >> 0x10) & 0xFFFF);
    f /= 65535.0f;
    D_00275C34[1] = r * 0x25 + 0xB;
    f += 0.0001f;
    func_001CFB50(D_0081F8F0, 0, arg0, *(float *)((char *)D_00275C34 + 0x54), f, 1.0f, 9.999999974752427e-07f, 10.0f);
    func_001CFBE0(arg1, 1, D_002573F0, D_0081F8F0, 1);

    r = D_00275C34[1];
    f = (float)((r >> 0x10) & 0xFFFF);
    f /= 65535.0f;
    D_00275C34[1] = r * 0x25 + 0xB;
    f += 0.0001f;
    func_001CFB50(D_0081F8F0, 0, arg0, *(float *)((char *)D_00275C34 + 0x54), f, 1.0f, 9.999999974752427e-07f, 10.0f);
    func_001CFBE0(arg1, 1, D_00257480, D_0081F8F0, 1);

    {
        float *p = (float *)((char *)D_00275C34 + 8);
        *p += (0.02f - *p) / 10.0f;
    }
    v = *(float *)((char *)D_00275C34 + 8);
    if (v < 0.02f) { v = 0.02f; }
    *(float *)((char *)D_00275C34 + 8) = v;
}
