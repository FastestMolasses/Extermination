// lui $v1,(0x10003020>>16); ori $v1,$v1,(0x10003020&0xFFFF); lw $v0,0x0($v1); andi $v0,$v0,0x4; jr $ra; srl $v0,$v0,2
asm int func_0011B3F8(void) {
    lui $3, 0x1000
    ori $3, $3, 0x3020
    lw $2, 0x0($3)
    andi $2, $2, 0x4
    jr $ra
    srl $2, $2, 2
}
