// Auto-recovered trivial leaf function. The boot ELF is stripped, so the
// name is the VRAM address until the real purpose is identified.
asm int func_001FE480(int *a0, int a1) {
    sll $v0, $a1, 4
    lw $a1, 0x0($a0)
    lw $v1, 0x8($a0)
    addu $a0, $a0, $a1
    addu $a1, $v1, $a0
    addiu $a0, $a1, 0x10
    lw $v1, 0x0($a1)
    addu $v0, $a0, $v0
    lw $v0, 0x0($v0)
    addu $v1, $a1, $v1
    jr $ra
    addu $v0, $v1, $v0
}
