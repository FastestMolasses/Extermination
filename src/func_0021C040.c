asm void func_0021C040(void) {
    lbu $3, 0x1F0($4)
    addiu $7, $0, 0x3C
    .word 0x14670030
    nop
    lbu $6, 0x3($5)
    addiu $3, $0, 0x5
    .word 0x54C30009
    addiu $7, $0, 0x2
    addiu $3, $0, 0x1
    sb $3, 0x4($4)
    sb $0, 0x5($4)
    sb $0, 0x6($4)
    sb $0, 0x1F0($4)
    .word 0x10000025
    sh $7, 0x20E($4)
    .word 0x24070002
    addiu $3, $0, 0xE
    sb $7, 0x4($4)
    sb $3, 0x5($4)
    addiu $3, $0, 0x3D
    sb $0, 0x6($4)
    addiu $6, $0, 0x1
    sb $3, 0x1F0($4)
    sb $6, 0x1F1($4)
    lbu $5, 0x3($5)
    .word 0x14A60004
    addiu $3, $0, 0x4
    .word 0x10000017
    sb $0, 0xD($4)
    .word 0x24030004
    .word 0x54A30005
    addiu $3, $0, 0x7
    sb $6, 0xD($4)
    .word 0x10000011
    sb $3, 0xF($4)
    .word 0x24030007
    .word 0x14A30006
    addiu $3, $0, 0x6
    addiu $3, $0, 0x8
    sb $7, 0xD($4)
    .word 0x1000000A
    sb $3, 0xF($4)
    .word 0x24030006
    .word 0x14A30006
    nop
    addiu $3, $0, 0x3
    sb $3, 0xD($4)
    addiu $3, $0, 0x9
    .word 0x10000002
    sb $3, 0xF($4)
    sb $0, 0xD($4)
    jr $ra
    nop
}
