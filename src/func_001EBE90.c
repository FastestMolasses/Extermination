// NEARMISS func_001EBE90  (vram 0x001EBE90, 0x78 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 79.80% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// instruction-scheduling wall. Residual is 3 DIFF_INSERT / 3 DIFF_DELETE of identical instructions (no arg mismatches): the list scheduler orders the independent float-constant materializations differently. Target hoists the 0.0f 'mtc1 zero,f16' to the top, builds f15(1e-6) before f14(1.0), and int...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// NEARMISS 80.0% (mwcc 991202) -- instruction-scheduling wall.
// Logic fully recovered and verified against the matched sibling func_001EAD70
// (identical func_001CFB50/func_001CFBE0 call pattern). This is a single-shot
// version: emits a particle/effect via func_001CFB50 with the two source floats
// at D_00275C34+0x54 and +0x5C plus constants 1.0f, 1e-6f, 0.0f, then dispatches
// func_001CFBE0(arg1, 1, &D_00256550, &D_0081F8F0, 0). Residual is pure
// list-scheduling order of the independent float-constant moves (3 INSERT /
// 3 DELETE of identical insns, no arg mismatches) -> permuter (scheduling).
// The 'float eps' temp is a scheduling steer that recovers the target's
// f15-before-f14 const order; it does not change semantics.
extern int *D_00275C34;
extern int D_00256550[2];
extern int D_0081F8F0[2];

void func_001CFB50(void *p, int b, int c, float f0, float f1, float f2, float f3, float f4);
void func_001CFBE0(int a, int b, void *c, void *d, int e);

void func_001EBE90(int arg0, int arg1) {
    float eps = 9.999999974752427e-07f;
    func_001CFB50(D_0081F8F0, 0, arg0,
                  *(float *)((char *)D_00275C34 + 0x54),
                  *(float *)((char *)D_00275C34 + 0x5C),
                  1.0f, eps, 0.0f);
    func_001CFBE0(arg1, 1, D_00256550, D_0081F8F0, 0);
}
