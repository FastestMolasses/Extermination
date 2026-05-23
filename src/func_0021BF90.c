extern void func_0017C370(void);

asm void func_0021BF90(void) {
    lbu $6, 0x0($4)
    addiu $7, $0, 0x2
    addiu $3, $0, 0xD
    addiu $2, $0, 0x3C
    ori $6, $6, 0x2
    sb $6, 0x0($4)
    sb $7, 0x4($4)
    sb $3, 0x5($4)
    sb $0, 0x6($4)
    addiu $6, $0, 0x1
    sb $2, 0x1F0($4)
    sb $6, 0x1F1($4)
    lbu $5, 0x3($5)
    .word 0x14A60005
    addiu $3, $0, 0x4
    sb $0, 0xD($4)
    .word 0x10000019
    sb $6, 0x25F($4)
    addiu $3, $0, 0x4
    .word 0x14A30004
    addiu $2, $0, 0x7
    .word 0x10000014
    sb $6, 0xD($4)
    addiu $2, $0, 0x7
    .word 0x14420005
    addiu $2, $0, 0x6
    sb $7, 0xD($4)
    .word 0x1000000E
    sb $6, 0x25F($4)
    addiu $2, $0, 0x6
    .word 0x14420006
    addiu $2, $0, 0x5
    addiu $2, $0, 0x3
    sb $2, 0xD($4)
    .word 0x10000007
    sb $6, 0x25F($4)
    addiu $2, $0, 0x5
    .word 0x14420003
    nop
    .word 0x10000002
    sb $3, 0xD($4)
    sb $0, 0xD($4)
    j func_0017C370
    nop
}
