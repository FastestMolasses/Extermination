// One-call non-leaf wrapper — asm void with extern decl for the
// callee. mwccmips emits the jal + R_MIPS_26 relocation for the
// referenced function symbol; the rest of the instructions are
// passed through verbatim.
extern void sceSifDmaStat(int, int, int, int);

asm void func_0010C920(void) {
    sll $a1, $a1, 24
    addiu $sp, $sp, -0x20
    sra $a1, $a1, 24
    sw $a0, 0x0($sp)
    sw $a1, 0x4($sp)
    addiu $a0, $zero, -0x7
    sd $ra, 0x10($sp)
    jal sceSifDmaStat
    daddu $a1, $sp, $zero
    ld $ra, 0x10($sp)
    jr $ra
    addiu $sp, $sp, 0x20
}
