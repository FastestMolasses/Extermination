// Auto-recovered trivial leaf function. The boot ELF is stripped, so the
// name is the VRAM address until the real purpose is identified.
asm void func_0010D928(int *a0, int *a1) {
    lw $v0, 0x10($a0)
    lw $a2, 0x1C($a1)
    lw $v1, 0x14($a0)
    sll $v0, $v0, 2
    addu $v0, $v0, $a2
    jr $ra
    sw $v1, 0x0($v0)
}
