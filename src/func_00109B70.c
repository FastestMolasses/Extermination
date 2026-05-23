// One-call non-leaf wrapper — asm void with extern decl for the
// callee. mwccmips emits the jal + R_MIPS_26 relocation for the
// referenced function symbol; the rest of the instructions are
// passed through verbatim.
extern void func_00109B20(int, int, int, int);

asm void func_00109B70(void) {
    addiu $sp, $sp, -0x30
    addiu $v0, $zero, 0x1
    sd $ra, 0x20($sp)
    daddu $a1, $sp, $zero
    jal func_00109B20
    sw $v0, 0x0($sp)
    ld $ra, 0x20($sp)
    jr $ra
    addiu $sp, $sp, 0x30
}
