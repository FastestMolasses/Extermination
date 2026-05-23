// One-call non-leaf wrapper — asm void with extern decl for the
// callee. mwccmips emits the jal + R_MIPS_26 relocation for the
// referenced function symbol; the rest of the instructions are
// passed through verbatim.
extern void func_00111188(int, int, int, int);

asm void func_001112A8(void) {
    addiu $sp, $sp, -0x10
    sd $ra, 0x0($sp)
    jal func_00111188
    nop
    lui $v1, (0x3FFFF >> 16)
    ld $ra, 0x0($sp)
    ori $v1, $v1, (0x3FFFF & 0xFFFF)
    xor $v0, $v0, $v1
    sltiu $v0, $v0, 0x1
    jr $ra
    addiu $sp, $sp, 0x10
}
