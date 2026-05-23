// lui $v1,(0x10003820>>16); ori $v1,$v1,(0x10003820&0xFFFF); jr $ra; lw $v0,0x0($v1)
asm int func_0011B6D0(void) {
    lui $3, 0x1000
    ori $3, $3, 0x3820
    jr $ra
    lw $2, 0x0($3)
}
