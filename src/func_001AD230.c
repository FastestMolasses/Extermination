// One-call non-leaf wrapper — asm void with extern decl for the
// callee. mwccmips emits the jal + R_MIPS_26 relocation for the
// referenced function symbol; the rest of the instructions are
// passed through verbatim.
extern void func_001AF2C0(int, int, int, int);

asm void func_001AD230(void) {
    addiu $sp, $sp, -0x10
    sq $ra, 0x0($sp)
    jal func_001AF2C0
    nop
    lq $ra, 0x0($sp)
    addiu $v0, $zero, 0x4
    jr $ra
    addiu $sp, $sp, 0x10
}
