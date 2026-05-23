// One-call non-leaf wrapper — asm void with extern decl for the
// callee. mwccmips emits the jal + R_MIPS_26 relocation for the
// referenced function symbol; the rest of the instructions are
// passed through verbatim.
extern void func_0011E938(int, int, int, int);

asm void func_00122ED0(void) {
    addiu $sp, $sp, -0x10
    sd $ra, 0x0($sp)
    lh $a1, 0xE($a0)
    jal func_0011E938
    lw $a0, 0x54($a0)
    ld $ra, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x10
}
