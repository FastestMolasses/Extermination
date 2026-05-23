// One-call non-leaf wrapper — asm void with extern decl for the
// callee. mwccmips emits the jal + R_MIPS_26 relocation for the
// referenced function symbol; the rest of the instructions are
// passed through verbatim.
extern void func_0011AE40(int, int, int, int);

asm void func_0011B028(void) {
    addiu $sp, $sp, -0x20
    addiu $a1, $zero, 0x43A
    sd $ra, 0x10($sp)
    jal func_0011AE40
    daddu $a0, $sp, $zero
    lhu $v0, 0x0($sp)
    ld $ra, 0x10($sp)
    jr $ra
    addiu $sp, $sp, 0x20
}
