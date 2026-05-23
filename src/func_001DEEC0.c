// One-call non-leaf wrapper — asm void with extern decl for the
// callee. mwccmips emits the jal + R_MIPS_26 relocation for the
// referenced function symbol; the rest of the instructions are
// passed through verbatim.
extern void func_001DEDB0(int, int, int, int);

asm void func_001DEEC0(void) {
    addiu $sp, $sp, -0x10
    sq $ra, 0x0($sp)
    jal func_001DEDB0
    nop
    sw $a1, 0x4($v0)
    lq $ra, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x10
}
