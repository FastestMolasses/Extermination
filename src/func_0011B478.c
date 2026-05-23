// One-call non-leaf wrapper — asm void with extern decl for the
// callee. mwccmips emits the jal + R_MIPS_26 relocation for the
// referenced function symbol; the rest of the instructions are
// passed through verbatim.
extern void func_0011B3F8(int, int, int, int);

asm void func_0011B478(void) {
    addiu $sp, $sp, -0x10
    sd $ra, 0x0($sp)
    jal func_0011B3F8
    nop
    ld $ra, 0x0($sp)
    andi $v0, $v0, 0x1
    jr $ra
    addiu $sp, $sp, 0x10
}
