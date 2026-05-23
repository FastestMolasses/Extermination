// Auto-recovered trivial leaf function. The boot ELF is stripped, so the
// name is the VRAM address until the real purpose is identified.
asm int func_00109CE0(int *a0) {
    lw $v0, 0x0($a0)
    lw $v1, 0x4($a0)
    lw $a1, 0x8($a0)
    addu $v0, $v0, $v1
    jr $ra
    subu $v0, $v0, $a1
}
