// One-call non-leaf wrapper — asm void with extern decl for the
// callee. mwccmips emits the jal + R_MIPS_26 relocation for the
// referenced function symbol; the rest of the instructions are
// passed through verbatim.
extern void func_001FB9F0(int, int, int, int);

asm void func_001575B0(void) {
    addiu $a1, $zero, 0x1000
    addiu $sp, $sp, -0x10
    addiu $a0, $zero, 0x3EF
    sq $ra, 0x0($sp)
    paddub $a2, $a1, $zero
    jal func_001FB9F0
    paddub $a3, $a1, $zero
    lq $ra, 0x0($sp)
    addiu $v0, $zero, 0x1
    jr $ra
    addiu $sp, $sp, 0x10
}
