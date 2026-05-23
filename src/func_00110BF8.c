// One-call non-leaf wrapper — asm void with extern decl for the
// callee. mwccmips emits the jal + R_MIPS_26 relocation for the
// referenced function symbol; the rest of the instructions are
// passed through verbatim.
extern void func_00110AB8(int, int, int, int);

asm void func_00110BF8(void) {
    addiu $sp, $sp, -0x20
    sd $s0, 0x0($sp)
    sd $ra, 0x10($sp)
    jal func_00110AB8
    daddu $s0, $a2, $zero
    sb $s0, 0x71($v0)
    ld $ra, 0x10($sp)
    addiu $v0, $zero, 0x1
    ld $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x20
}
