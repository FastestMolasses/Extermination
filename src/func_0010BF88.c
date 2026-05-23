// One-call non-leaf wrapper — asm void with extern decl for the
// callee. mwccmips emits the jal + R_MIPS_26 relocation for the
// referenced function symbol; the rest of the instructions are
// passed through verbatim.
extern void func_0011FD78(int, int, int, int);

asm void func_0010BF88(void) {
    addiu $sp, $sp, -0x10
    sd $ra, 0x0($sp)
    jal func_0011FD78
    nop
    addiu $v1, $zero, 0x5
    ld $ra, 0x0($sp)
    sw $v1, 0x0($v0)
    addiu $v0, $zero, -0x1
    jr $ra
    addiu $sp, $sp, 0x10
}
