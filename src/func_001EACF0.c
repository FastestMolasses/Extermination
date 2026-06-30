// NEARMISS func_001EACF0  (vram 0x001EACF0, 0x7C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 80.61% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FP-constant scheduling permutation: target hoists the multi-instruction 1e-6f const (lui+ori+mtc1) before the paddub s0,a1 arg-save and emits the single-instruction 1.0f const after it; mwcc233 does the reverse (1.0f const first, 1e-6f split around paddub). Body/logic 100% correct, register color...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// NEARMISS 80.6% (mwcc233; 991202=74.2%). Animation/interpolation setup wrapper,
// sibling of the verified-100% func_001EAF80. Forwards two floats from the gp-rel
// global object D_00275C34 (fields +0x54 fade, +0x5C) plus FPU constants 1.0f,
// 1e-6f (0x358637BD), 3.0f into func_001CFB50(&D_0081F8F0,...), then a single
// func_001CFBE0(arg1, 0, D_00255620, &D_0081F8F0, 0). sq/lq frame saves $ra,$s0
// (s0 = arg1 held across both calls). func_001CFBE0 genuinely takes 5 args.
// WALL: target schedules the 1e-6f const (lui+ori+mtc1) BEFORE the paddub s0,a1
// and the 1.0f const after; mwcc emits 1.0f first, splitting 1e-6f around paddub.
// Pure FP-const scheduling/coloring permutation -> dedicated permuter pass.
extern char *D_00275C34;
extern int D_00255620[8];
extern int D_0081F8F0[8];
extern void func_001CFB50(int *, int, int, float, float, float, float, float);
extern void func_001CFBE0(int, int, int *, int *, int);

void func_001EACF0(int arg0, int arg1) {
    func_001CFB50(D_0081F8F0, 0, arg0,
                  *(float *)(D_00275C34 + 0x54),
                  *(float *)(D_00275C34 + 0x5C),
                  1.0f, 9.999999974752427e-07f, 3.0f);
    func_001CFBE0(arg1, 0, D_00255620, D_0081F8F0, 0);
}
