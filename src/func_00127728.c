// One-call non-leaf wrapper — asm void with extern decl for the
// callee. mwccmips emits the jal + R_MIPS_26 relocation for the
// referenced function symbol; the rest of the instructions are
// passed through verbatim.
extern void func_00126AB8(int, int, int, int);

asm void func_00127728(void) {
    addiu $sp, $sp, -0x30
    sw $a0, 0x0($sp)
    sd $ra, 0x20($sp)
    daddu $a0, $sp, $zero
    sw $a1, 0x4($sp)
    sw $a2, 0x8($sp)
    jal func_00126AB8
    sd $a3, 0x10($sp)
    ld $ra, 0x20($sp)
    jr $ra
    addiu $sp, $sp, 0x30
}
