// Auto-recovered trivial leaf function. The boot ELF is stripped, so the
// name is the VRAM address until the real purpose is identified.
// Matched via asm void: loop uses sltu/beqz pattern with nops that mwcc
// does not reproduce from this C at -O4,p.
asm void func_00225CF0(unsigned char *a0, unsigned int a1, unsigned int a2) {
    sltu $at, $a2, $a1
    bnez $at, cf0_end
    nop
cf0_loop:
    addu $v1, $a0, $a1
    addiu $a1, $a1, 1
    sltu $at, $a2, $a1
    sb $zero, 0($v1)
    nop
    beqz $at, cf0_loop
    nop
cf0_end:
    jr $ra
    nop
}
