// Auto-recovered trivial leaf function. The boot ELF is stripped, so the
// name is the VRAM address until the real purpose is identified.
asm int func_0010A4D8(int *a0, int a1, int a2) {
    sra $v0, $a1, 4
    sra $v1, $a2, 4
    sw $v0, 0xC($a0)
    sw $v1, 0x10($a0)
    addiu $v0, $zero, 1
    sw $a1, 0x4($a0)
    jr $ra
    sw $a2, 0x8($a0)
}
