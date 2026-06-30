// NEARMISS func_001ECFB0  (vram 0x001ECFB0, 0x14C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 67.36% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-coloring + scheduling near-miss (best on mwcc 991202, 84.1%; 233 only 67.4%). Body/structure fully correct. D_00275C34 correctly gp-rel (-sdatathreshold 8) and D_002571B0/D_0081F8F0 kept %hi/%lo absolute (declared as large arrays). Residuals: GPR coloring (loop counter / slot pointer swa...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

//
// Runs three particle/emitter slots from the table at D_002571B0 (stride 0x90).
// Each iteration pulls a 32-bit LCG-ish counter from D_00275C34+4, advances it
// (n*37+11), derives a normalized float ((n>>16)&0xFFFF)/65535 + 0.0001, and
// feeds func_001CFB50(&D_0081F8F0, 0, arg0, <field 0x54>, frac, 1.0f, 1e-6f,
// 3.0f) then func_001CFBE0(arg1, 1, slot, &D_0081F8F0, 0). Finally eases the
// float at D_00275C34+8 toward 0.02 (delta/10) and clamps it to >= 0.02.
//
// NEARMISS 84.1% (mwcc 991202; tables declared large to stay %hi/%lo absolute,
// D_00275C34 gp-rel via -sdatathreshold 8). Logic fully recovered; sole residual
// is GPR coloring (loop counter/slot pointer s0<->s1, n in a0-vs-a1), FP-register
// coloring, and the int-multiply / float-div schedule interleave. Permuter class.
extern void func_001CFB50(int a, int b, int c, float f0, float f1, float f2, float f3, float f4);
extern void func_001CFBE0(int a, int b, int c, int d, int e);
extern int D_002571B0[0x100];
extern char *D_00275C34;
extern int D_0081F8F0[0x100];

void func_001ECFB0(int arg0, int arg1) {
    int *slot;
    int n;
    int i;
    float v;
    float frac;

    slot = D_002571B0;
    for (i = 0; i < 3; i++) {
        n = *(int *)(D_00275C34 + 4);
        frac = (float)((n >> 16) & 0xFFFF) / 65535.0f + 0.0001f;
        *(int *)(D_00275C34 + 4) = n * 37 + 11;
        func_001CFB50((int)D_0081F8F0, 0, arg0, *(float *)(D_00275C34 + 0x54),
                      frac, 1.0f, 1e-6f, 3.0f);
        func_001CFBE0(arg1, 1, (int)slot, (int)D_0081F8F0, 0);
        slot = (int *)((char *)slot + 0x90);
    }
    v = *(float *)(D_00275C34 + 8);
    v = v + (0.02f - v) / 10.0f;
    *(float *)(D_00275C34 + 8) = v;
    v = *(float *)(D_00275C34 + 8);
    if (v < 0.02f) {
        v = 0.02f;
    }
    *(float *)(D_00275C34 + 8) = v;
}
