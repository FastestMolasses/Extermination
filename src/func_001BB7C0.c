// One-call non-leaf wrapper — asm void with extern decl for the
// callee. mwccmips emits the jal + R_MIPS_26 relocation for the
// referenced function symbol; the rest of the instructions are
// passed through verbatim.
extern void func_001BA1F0(int, int, int, int);

asm void func_001BB7C0(void) {
    addiu $sp, $sp, -0x10
    sq $ra, 0x0($sp)
    jal func_001BA1F0
    nop
    lq $ra, 0x0($sp)
    addiu $v1, $zero, 0x1
    movz $v1, $zero, $v0
    paddub $v0, $v1, $zero
    jr $ra
    addiu $sp, $sp, 0x10
}
