// Auto-recovered trivial leaf function. The boot ELF is stripped, so the
// name is the VRAM address until the real purpose is identified.
asm int func_00109AF8(int *a0, int a1, int a2, int a3) {
    lw $v0, 0x40($a0)
    sll $a1, $a1, 3
    addiu $v1, $v0, 12
    addu $v0, $v0, $a1
    addu $v1, $v1, $a1
    sw $a3, 16($v0)
    lw $v0, 0($v1)
    jr $ra
    sw $a2, 0($v1)
}
