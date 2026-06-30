// NEARMISS func_001ECE70  (vram 0x001ECE70, 0x7C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 80.58% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FP-constant emit-order/instruction-scheduling permutation (same class as func_001EC3F0/func_001EBBB0). CW target emits f15(1e-6f) before paddub s0,a1 and before f14(1.0f); mwcc emits f14-first. Registers/constants/args/frame byte-exact; only constant-load block order differs. Permuter territory.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

//
// Animation/effect setup wrapper. Pulls two floats (+0x54 fade, +0x5C) from the gp-rel global state
// record *D_00275C34 and calls func_001CFB50(&D_0081F8F0, 0, arg0, f54, f5C, 1.0f, ~1e-6f, 9.0f), then
// fires one func_001CFBE0(arg1, 1, D_00257000, &D_0081F8F0, 0) event on the same D_0081F8F0 object.
// 0x358637BD = 9.999999974752427e-07; 0x41100000 = 9.0f. sq/lq frame saves $ra,$s0 (s0 = arg1 held
// across the calls). func_001CFBE0 mode = 1, trailing arg 0.
//
// WALL: FP-constant emit-order / instruction-scheduling permutation. The CW target emits the f15 (1e-6f,
// 2-instr lui+ori) constant BEFORE the arg1-save 'paddub s0,a1' and before the f14 (1.0f) constant; mwcc
// 2.3.3 (and 991202) deterministically emit f14(1.0)-first then f15(1e-6) with the paddub between them.
// Registers (f14=1.0,f15=1e-6,f16=9.0,t0=0) and all operand bytes identical -- only the constant-load
// block order differs. Not the clean-store nop, so 2.3.3 cannot fix it -> register/scheduling-permuter territory.
extern char D_0081F8F0[0x100];
extern char D_00257000[0x100];
extern int *D_00275C34;
extern void func_001CFB50(void *, int, int, float, float, float, float, float);
extern void func_001CFBE0(int, int, void *, void *, int);

void func_001ECE70(int arg0, int arg1) {
    func_001CFB50(D_0081F8F0, 0, arg0,
                  *(float *)((char *)D_00275C34 + 0x54),
                  *(float *)((char *)D_00275C34 + 0x5C),
                  1.0f, 9.999999974752427e-07f, 9.0f);
    func_001CFBE0(arg1, 1, D_00257000, D_0081F8F0, 0);
}
