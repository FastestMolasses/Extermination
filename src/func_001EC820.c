// NEARMISS func_001EC820  (vram 0x001EC820, 0x180 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 66.71% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FPU register-coloring + commutative add.s operand-order permutation (identical class to func_001EC470). CW colors the int->float chain in $f13; mwcc spills through f2->f1 and picks a3 vs a2 for the +0x54 load. All constants/args/frame/branches identical. Not the clean-store nop (233 is worse). Re...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

extern char *D_00275C34;
extern int D_0081F8F0[8];
extern int D_00256C10[8];
extern int D_00256CA0[8];

extern void func_001CFB50(int *p, int z, int idx, float f12, float f13,
                          float f14, float f15, float f16);
extern void func_001CFBE0(int a, int b, int *c, int *d, int e);

void func_001EC820(int arg0, int arg1) {
    int v;
    float fr;

    v = *(int *)(D_00275C34 + 4);
    fr = (float)((v >> 16) & 0xFFFF) / 65535.0f + 0.0001f;
    *(int *)(D_00275C34 + 4) = v * 0x25 + 0xB;
    func_001CFB50(D_0081F8F0, 0, arg0,
                  *(float *)(D_00275C34 + 0x54), fr,
                  1.0f, 1e-6f, 3.0f);
    func_001CFBE0(arg1, 1, D_00256C10, D_0081F8F0, 0);

    v = *(int *)(D_00275C34 + 4);
    fr = (float)((v >> 16) & 0xFFFF) / 65535.0f + 0.0001f;
    *(int *)(D_00275C34 + 4) = v * 0x25 + 0xB;
    func_001CFB50(D_0081F8F0, 0, arg0,
                  *(float *)(D_00275C34 + 0x54), fr,
                  1.0f, 1e-6f, 3.0f);
    func_001CFBE0(arg1, 1, D_00256CA0, D_0081F8F0, 0);
}
