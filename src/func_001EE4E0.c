// NEARMISS func_001EE4E0  (vram 0x001EE4E0, 0x350 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 95.99% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Remaining differences: only the order in which the three constant arguments (1.0f, 1e-6f, 15.0f) of the
// three func_001CFB50 calls are set up. The original rotates them per call (call 1 sets up 1e-6/15 first,
// call 2 1.0 first, call 3 15.0 first; the same rotation is in every 3-call sibling func_001ED100..func_001EF1C0),
// while this file alone emits source order in all three. The order is translation-unit state, not this
// function's source: compiling the byte-matched func_001ECEF0 earlier in the same file (diagnostic only) already
// reproduces call 3's order (98.97%), but constant-only throwaway predecessors and copies of the sibling template
// do not change it. The real immediate predecessor func_001EE190 is itself a NEARMISS with the same residual, so
// a `// TU_STATIC: func_001EE190` retry belongs after that function is matched. Not fixed by constant locals in
// either declaration order, double literals, the one-statement random expression, a 3-iteration loop over the
// 0x90-byte record array, or compilers 991202 (86.34%) / 2.4 (82.18%).
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// SEMANTICS: Effect/animation setup, sibling of func_001EE190 / func_001EF1C0 (same template). Rewrites the
// 8-float parameter blocks at offset 0x20 of three 0x90-byte records (D_002576C0 / D_00257750 / D_002577E0)
// to 12/12/16/0/0/0/0/0 and twice 32/32/64/96/16/16/64/0, then for each record:
//     r = state->index4;                              // D_00275C34[1]
//     phase = (float)((r >> 16) & 0xFFFF) / 65535.0f + 1e-4f;
//     state->index4 = r * 37 + 11;                    // LCG advance
//     func_001CFB50(D_0081F8F0, 0, arg0, state->f54, phase, 1.0f, 1e-6f, 15.0f);
//     func_001CFBE0(arg1, mode, record, D_0081F8F0, 1);   // mode 1, 6, 1
// Finally eases state->f8 toward 0.02 by a tenth of the gap and clamps it to a 0.02 floor.
// The parameter blocks are volatile float[8] so they stay absolute (per-word relocations) and are
// stored in address order; D_00275C34 stays gp-relative (-sdatathreshold 4).
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

void func_001EE4E0(int arg0, int arg1) {
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
    func_001CFBE0(arg1, 6, D_00257750, D_0081F8F0, 1);
    r = D_00275C34[1];
    f = (float)((r >> 0x10) & 0xFFFF);
    f /= 65535.0f;
    D_00275C34[1] = r * 0x25 + 0xB;
    f += 0.0001f;
    func_001CFB50(D_0081F8F0, 0, arg0, *(float *)((char *)D_00275C34 + 0x54), f, 1.0f, 9.999999974752427e-07f, 15.0f);
    func_001CFBE0(arg1, 1, D_002577E0, D_0081F8F0, 1);
    {
        float *p = (float *)((char *)D_00275C34 + 8);
        *p += (0.02f - *p) / 10.0f;
    }
    v = *(float *)((char *)D_00275C34 + 8);
    if (v < 0.02f) { v = 0.02f; }
    *(float *)((char *)D_00275C34 + 8) = v;
}
