// One-call non-leaf wrapper — asm void with extern decl for the
// callee. mwccmips emits the jal + R_MIPS_26 relocation for the
// referenced function symbol; the rest of the instructions are
// passed through verbatim.
extern void func_00110048(int, int, int, int);

asm void func_00110150(void) {
    addiu $sp, $sp, -0x10
    sd $ra, 0x0($sp)
    jal func_00110048
    addiu $a3, $zero, 0x1
    ld $ra, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x10
}
