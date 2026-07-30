// NEARMISS sub_sequenceScalableExtension_is_not  (vram 0x0010AD48, 0xC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 0.00% via mwcc 2.3 (mwcps2-2.3-991202) (-O4,p -sdatathreshold 0). The LOGIC is
// faithful and complete; the residual is a genuine compiler artifact no source change fixes:
// tail-call/sibling-call trampoline — the target is 3 instructions (lui / j func / addiu in
// the delay slot); mwcc has no way to emit a tail `j` from C and always emits jal + epilogue,
// so essentially every instruction differs and the percentage is meaningless here.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc
// CFLAGS: -O4,p -sdatathreshold 0

extern int D_0026B6F8[];
extern void func_0010A3A8(int *);

void sub_sequenceScalableExtension_is_not(void) {
    int *a0 = D_0026B6F8;
    func_0010A3A8(a0);
}
