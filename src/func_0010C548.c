// One-call non-leaf wrapper — asm void with extern decl for the
// callee. mwccmips emits the jal + R_MIPS_26 relocation for the
// referenced function symbol; the rest of the instructions are
// passed through verbatim.
extern void _iEnableDmac(int, int, int, int);

asm void func_0010C548(void) {
    addiu $sp, $sp, -0x10
    sd $ra, 0x0($sp)
    jal _iEnableDmac
    nop
    sync
    ld $ra, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x10
}
