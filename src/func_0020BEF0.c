// Auto-recovered trivial leaf function. The boot ELF is stripped, so the
// name is the VRAM address until the real purpose is identified.
asm int func_0020BEF0(unsigned char *a0) {
    lbu $v1, 0x17($a0)
    lbu $v0, 0x19($a0)
    lbu $a0, 0x18($a0)
    addu $v0, $v1, $v0
    slt $v1, $v0, $a0
    bnez $v1, bef0_end
    nop
    subu $v0, $v0, $a0
bef0_end:
    jr $ra
    nop
}
