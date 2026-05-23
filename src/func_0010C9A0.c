// One-call non-leaf wrapper — asm void with extern decl for the
// callee. mwccmips emits the jal + R_MIPS_26 relocation for the
// referenced function symbol; the rest of the instructions are
// passed through verbatim.
extern void sceSifDmaStat(int, int, int, int);

asm void func_0010C9A0(void) {
    addiu $sp, $sp, -0x20
    sw $a0, 0x0($sp)
    daddu $a1, $sp, $zero
    sd $ra, 0x10($sp)
    jal sceSifDmaStat
    addiu $a0, $zero, 0x10
    ld $ra, 0x10($sp)
    jr $ra
    addiu $sp, $sp, 0x20
}
