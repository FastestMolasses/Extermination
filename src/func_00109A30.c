// Auto-recovered trivial leaf function. The boot ELF is stripped, so the
// name is the VRAM address until the real purpose is identified.
asm int func_00109A30(int *a0) {
    lw $v1, 0x40($a0)
    jr $ra
    lw $v0, 0x0($v1)
}
