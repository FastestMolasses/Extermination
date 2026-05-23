// lui $v1,(0x10003C20>>16); ori $v1,$v1,(0x10003C20&0xFFFF); jr $ra; lw $v0,0x0($v1)
asm int func_0011BA00(void) {
    lui $3, 0x1000
    ori $3, $3, 0x3C20
    jr $ra
    lw $2, 0x0($3)
}
