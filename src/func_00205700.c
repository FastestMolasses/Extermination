extern void func_00100550(void);

asm void func_00205700(void) {
    .word 0x10A00003
    nop
    .word 0x10000003
    lw $2, 0x2C($4)
    lw $2, 0x28($4)
    nop
    lhu $5, 0x10($4)
    andi $3, $2, 0x1FF
    addiu $2, $0, -0x200
    and $2, $5, $2
    or $2, $2, $3
    j func_00100550
    sh $2, 0x10($4)
}
