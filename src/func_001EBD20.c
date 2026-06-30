// NEARMISS func_001EBD20  (vram 0x001EBD20, 0xE4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 73.30% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FP-constant emit-order / instruction-scheduling permutation, identical to func_001EBC30 (same template, constants 5.0f/6.0f, tables D_00256430/D_002563A0). Body 100% logically correct, frame + registers + operands byte-exact; only the constant-load + gp-load scheduling order differs. Not the clea...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// NEARMISS func_001EBD20 (vram 0x001EBD20, 0xE4 bytes) — readable decompilation, NOT byte-identical.
// 73.3% via mwcc 2.3.3 (best); 68.8% via mwcc 991202. Logic + structure fully recovered.
//
// Identical template to func_001EBC30 (copy_qw4 + 0x70003434 bump + func_001CFB50 + if/else
// func_001CFBE0), differing only in constants and tables: bump is 5.0f, the final func_001CFB50
// FP constant is 6.0f (0x40C00000), and the func_001CFBE0 tables are D_00256430 (flag==0) /
// D_002563A0 (else). D_70003400 reloc symbol; the +0x34 float access is a LITERAL absolute
// address 0x70003434 (lui 0x7000 / 0x3434). D_00275C34 / D_00275C30 gp-relative.
//
// WALL: same FP-constant emit-order / instruction-scheduling permutation as func_001EBC30 and the
// parked siblings func_001EC1F0 / func_001EC3F0. Registers/operands/args/frame byte-exact; only
// constant-load + gp-load scheduling order differs. Not the clean-store nop -> permuter territory.
extern char D_0081F8F0[0x100];
extern char D_00256430[0x100];
extern char D_002563A0[0x100];
extern char *D_00275C30;
extern char *D_00275C34;
extern float D_70003400[4];

extern void copy_qw4(float *dst, int src);
extern void func_001CFB50(void *, int, void *, float, float, float, float, float);
extern void func_001CFBE0(int, int, void *, void *, int);

void func_001EBD20(int arg0, int arg1) {
    copy_qw4(D_70003400, arg0);
    *(float *)0x70003434 = *(float *)0x70003434 + 5.0f;
    func_001CFB50(D_0081F8F0, 0, D_70003400,
                  *(float *)(D_00275C34 + 0x54),
                  *(float *)(D_00275C34 + 0x5C),
                  1.0f, 9.999999974752427e-07f, 6.0f);
    if (*(int *)(D_00275C30 + 0x38) == 0) {
        func_001CFBE0(arg1, 1, D_00256430, D_0081F8F0, 0);
        return;
    }
    func_001CFBE0(arg1, 1, D_002563A0, D_0081F8F0, 0);
}
