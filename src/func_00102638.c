// Auto-recovered trivial leaf function. The boot ELF is stripped, so the
// name is the VRAM address until the real purpose is identified.
asm int func_00102638(int *a0) {
    lw $v0, 0x0($a0)
    addiu $v1, $zero, -257
    and $v1, $v0, $v1
    srl $v0, $v0, 8
    sw $v1, 0x0($a0)
    jr $ra
    andi $v0, $v0, 1
}
