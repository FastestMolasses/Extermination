// NEARMISS func_001EC1F0  (vram 0x001EC1F0, 0x7C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 76.87% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Body 100% correct (93.5% mwcc991202). Two residuals, both pure scheduling artifacts no C change fixes: (1) second-call arg1->a0 register move (paddub a0,s0) emitted eagerly by mwcc vs delayed-to-last by CW target — the paddub-timing class that the 100% sibling func_001EAF80 only resolves via a se...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// NEARMISS func_001EC1F0  (vram 0x001EC1F0, 0x7C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 93.5% via mwcc 991202 (-O4,p -sdatathreshold 4); 80.4% via mwcc 2.3.3.
// The LOGIC and STRUCTURE are faithful and fully recovered; the residual diff is a
// genuine compiler scheduling artifact that no source change fixes here.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// Animation/effect setup wrapper, sibling of the verified-100% func_001EAF80 and the
// 80.6% single-call nearmisses func_001EACF0/func_001EBBB0/func_001EBE10 (same template,
// this one uses const 5.0f, table D_00256700, flag 1). Pulls two floats from the gp-rel
// global object *D_00275C34 (fields +0x54 and +0x5C) plus FPU constants 1.0f, 1e-6f
// (0x358637BD), 5.0f into func_001CFB50(&D_0081F8F0, 0, arg0, ...), then a single
// func_001CFBE0(arg1, 1, D_00256700, &D_0081F8F0, 0). sq/lq frame saves $ra,$s0
// (s0 = arg1 held across the two calls). func_001CFBE0 genuinely takes 5 args.
//
// The float-arg locals (one/five/f54/tiny/f5c) reproduce the target's FP-constant
// emit/eval order around the paddub s0,a1 arg1-save (mwcc 991202: 93.5%).
//
// WALL: pure scheduling permutation, two residual deltas, both unreachable from C:
//   (1) the second call's arg1->a0 register move (paddub a0,s0) — mwcc schedules it
//       eagerly right after the a2 setup; the CW target delays it to the last setup
//       slot before the jal (the same paddub-timing the 100% sibling func_001EAF80
//       only resolves because it has a SECOND func_001CFBE0 call).
//   (2) a trailing CW function-alignment nop (0x7C -> 0x80) mwcc does not emit.
// Single-call wrapper of this template is the documented FP-const/paddub-timing
// scheduling-permutation class -> dedicated permuter pass.
extern char *D_00275C34;
extern int D_00256700[8];
extern int D_0081F8F0[8];
extern void func_001CFB50(int *, int, int, float, float, float, float, float);
extern void func_001CFBE0(int, int, int *, int *, int);

void func_001EC1F0(int arg0, int arg1) {
    float one = 1.0f;
    float five = 5.0f;
    float f54 = *(float *)(D_00275C34 + 0x54);
    float tiny = 1e-6f;
    float f5c = *(float *)(D_00275C34 + 0x5C);
    func_001CFB50(D_0081F8F0, 0, arg0, f54, f5c, one, tiny, five);
    func_001CFBE0(arg1, 1, D_00256700, D_0081F8F0, 0);
}
