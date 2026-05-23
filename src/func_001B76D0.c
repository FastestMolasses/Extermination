// One-call non-leaf wrapper — asm void with extern decl for the
// callee. mwccmips emits the jal + R_MIPS_26 relocation for the
// referenced function symbol; the rest of the instructions are
// passed through verbatim.
extern void func_001B1E20(int, int, int, int);

asm void func_001B76D0(void) {
    addiu $sp, $sp, -0x10
    sq $ra, 0x0($sp)
    lw $a1, 0x18($a2)
    jal func_001B1E20
    lw $a0, 0x14($a2)
    lq $ra, 0x0($sp)
    addiu $v0, $zero, 0x1
    jr $ra
    addiu $sp, $sp, 0x10
}
