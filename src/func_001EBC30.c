// NEARMISS func_001EBC30  (vram 0x001EBC30, 0xE4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 73.30% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FP-constant emit-order / instruction-scheduling permutation. Body 100% logically correct, frame + registers + operands byte-exact; CW target loads D_00275C34 into a caller-temp once and interleaves the FP constants (1.0/1e-6/3.0) around the 0x70003434 store in a specific order, mwcc reorders the ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// NEARMISS func_001EBC30 (vram 0x001EBC30, 0xE4 bytes) — readable decompilation, NOT byte-identical.
// 73.3% via mwcc 2.3.3 (best); 68.8% via mwcc 991202. Logic + structure fully recovered.
//
// Animation/effect setup wrapper, copy_qw4 + if/else variant of the func_001EC1F0 / func_001EC3F0
// family. Copies a 4-word quad into the scratch object D_70003400 (filled from arg0), bumps the
// float at the literal absolute address 0x70003434 by 2.5f, then calls
// func_001CFB50(&D_0081F8F0, 0, &D_70003400, fade=*(D_00275C34+0x54), *(D_00275C34+0x5C),
// 1.0f, 9.999999974752427e-07f (0x358637BD), 3.0f). It then fires one func_001CFBE0 event on
// the same D_0081F8F0 object, choosing the table by the gp-rel global flag *(D_00275C30+0x38):
// ==0 -> D_00256310, else -> D_00256280. sq/lq frame saves $ra,$s0 (s0 = arg1 held across calls).
// D_70003400 is a relocated symbol (passed to copy_qw4/func_001CFB50); the +0x34 float access is
// emitted as a LITERAL absolute address 0x70003434 (lui 0x7000 / 0x3434), matching the target.
// D_00275C34 / D_00275C30 are gp-relative (sdatathreshold 4).
//
// WALL: FP-constant emit-order / instruction-scheduling permutation (same documented class as the
// parked siblings func_001EC1F0 80.4-93.5% and func_001EC3F0 80.6%). The CW target loads
// D_00275C34 into a caller-temp ($a1) right after the 2.5f add and reuses it for both float
// loads, and interleaves the FP constants (1.0f, 1e-6f, 3.0f) in a specific order around the
// 0x70003434 store; mwcc deterministically reorders the constant-load blocks and the gp-load.
// Hoisting D_00275C34 to a local recovers the single-load but promotes it to a callee-saved reg
// and grows the frame 0x20->0x30 (wrong); inline keeps the correct frame. Registers/operands/
// args all byte-exact — only constant-load + gp-load scheduling differs. Not the clean-store nop,
// so 2.3.3 cannot fix it -> register/scheduling-permuter territory.
extern char D_0081F8F0[0x100];
extern char D_00256310[0x100];
extern char D_00256280[0x100];
extern char *D_00275C30;
extern char *D_00275C34;
extern float D_70003400[4];

extern void copy_qw4(float *dst, int src);
extern void func_001CFB50(void *, int, void *, float, float, float, float, float);
extern void func_001CFBE0(int, int, void *, void *, int);

void func_001EBC30(int arg0, int arg1) {
    copy_qw4(D_70003400, arg0);
    *(float *)0x70003434 = *(float *)0x70003434 + 2.5f;
    func_001CFB50(D_0081F8F0, 0, D_70003400,
                  *(float *)(D_00275C34 + 0x54),
                  *(float *)(D_00275C34 + 0x5C),
                  1.0f, 9.999999974752427e-07f, 3.0f);
    if (*(int *)(D_00275C30 + 0x38) == 0) {
        func_001CFBE0(arg1, 1, D_00256310, D_0081F8F0, 0);
        return;
    }
    func_001CFBE0(arg1, 1, D_00256280, D_0081F8F0, 0);
}
