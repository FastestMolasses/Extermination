// Auto-recovered trivial leaf function. The boot ELF is stripped, so the
// name is the VRAM address until the real purpose is identified.
asm void func_0010C008(int *a0) {
    lui $v0, 0x20
    sw $zero, 8($a0)
    sw $v0, 0($a0)
    jr $ra
    sw $zero, 4($a0)
}
