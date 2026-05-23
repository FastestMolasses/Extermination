// Auto-recovered trivial leaf function. The boot ELF is stripped, so the
// name is the VRAM address until the real purpose is identified.
asm void func_00121BA0(int *a0, int *a1) {
    beqz $a1, ba0_end
    nop
    lw $v0, 0x4($a1)
    lw $a0, 0x4C($a0)
    sll $v0, $v0, 2
    addu $v0, $v0, $a0
    lw $v1, 0x0($v0)
    sw $v1, 0x0($a1)
    sw $a1, 0x0($v0)
ba0_end:
    jr $ra
    nop
}
