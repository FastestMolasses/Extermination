// Auto-recovered trivial leaf function. The boot ELF is stripped, so the
// name is the VRAM address until the real purpose is identified.
// Uses asm void to preserve dead instruction after unconditional branch.
asm void func_001B5C90(void) {
    andi $v0, $a0, 0xFF
    addiu $v0, $v0, 2
    andi $v1, $v0, 0xFFFF
    slti $v0, $v1, 0x100
    .word 0x14400004
    andi $v0, $v1, 0xFC
    .word 0x10000002
    addiu $v0, $zero, 0xFC
    andi $v0, $v1, 0xFC
    jr $ra
    nop
}
