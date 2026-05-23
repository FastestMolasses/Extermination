// Auto-recovered trivial leaf function. The boot ELF is stripped, so the
// name is the VRAM address until the real purpose is identified.
asm void func_00109A10(int *a0, int *a1, int *a2, int *a3) {
    lw $8, 0x40($a0)
    lw $v0, 0x94($8)
    sw $v0, 0x0($a1)
    lw $v1, 0x98($8)
    sw $v1, 0x0($a2)
    lw $v0, 0x9C($8)
    jr $ra
    sw $v0, 0x0($a3)
}
