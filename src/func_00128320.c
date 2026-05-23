// One-call non-leaf wrapper — asm void with extern decl for the
// callee. mwccmips emits the jal + R_MIPS_26 relocation for the
// referenced function symbol; the rest of the instructions are
// passed through verbatim.
extern void func_001277B0(int, int, int, int);

asm void func_00128320(void) {
    addiu $sp, $sp, -0x20
    sw $a0, 0x0($sp)
    sd $ra, 0x10($sp)
    daddu $a0, $sp, $zero
    sw $a1, 0x4($sp)
    sw $a2, 0x8($sp)
    jal func_001277B0
    sw $a3, 0xC($sp)
    ld $ra, 0x10($sp)
    jr $ra
    addiu $sp, $sp, 0x20
}
