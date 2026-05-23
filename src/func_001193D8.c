// One-call non-leaf wrapper — asm void with extern decl for the
// callee. mwccmips emits the jal + R_MIPS_26 relocation for the
// referenced function symbol; the rest of the instructions are
// passed through verbatim.
extern void func_00119400(int, int, int, int);

asm void func_001193D8(void) {
    daddu $a3, $a2, $zero
    addiu $sp, $sp, -0x10
    daddu $a2, $a0, $zero
    sd $ra, 0x0($sp)
    jal func_00119400
    addiu $a0, $zero, 0x21
    ld $ra, 0x0($sp)
    daddu $v0, $zero, $zero
    jr $ra
    addiu $sp, $sp, 0x10
}
