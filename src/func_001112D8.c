// One-call non-leaf wrapper — asm void with extern decl for the
// callee. mwccmips emits the jal + R_MIPS_26 relocation for the
// referenced function symbol; the rest of the instructions are
// passed through verbatim.
extern void func_001111F8(int, int, int, int);

asm void func_001112D8(void) {
    addiu $sp, $sp, -0x10
    sd $ra, 0x0($sp)
    jal func_001111F8
    addiu $a2, $zero, 0xFFF
    ld $ra, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x10
}
