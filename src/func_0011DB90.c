// One-call non-leaf wrapper — asm void with extern decl for the
// callee. mwccmips emits the jal + R_MIPS_26 relocation for the
// referenced function symbol; the rest of the instructions are
// passed through verbatim.
extern void func_001274B0(int, int, int, int);

asm void func_0011DB90(void) {
    addiu $sp, $sp, -0x10
    sd $ra, 0x0($sp)
    ld $a0, 0x8($a0)
    jal func_001274B0
    daddu $a1, $a0, $zero
    ld $ra, 0x0($sp)
    daddu $v0, $zero, $zero
    jr $ra
    addiu $sp, $sp, 0x10
}
