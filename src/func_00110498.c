// One-call non-leaf wrapper — asm void with extern decl for the
// callee. mwccmips emits the jal + R_MIPS_26 relocation for the
// referenced function symbol; the rest of the instructions are
// passed through verbatim.
extern void RFU116(int, int, int, int);

asm void func_00110498(void) {
    addiu $sp, $sp, -0x10
    sd $ra, 0x0($sp)
    jal RFU116
    addiu $a0, $zero, 0x4
    lui $v1, (0x10000 >> 16)
    ld $ra, 0x0($sp)
    and $v0, $v0, $v1
    sltu $v0, $zero, $v0
    jr $ra
    addiu $sp, $sp, 0x10
}
