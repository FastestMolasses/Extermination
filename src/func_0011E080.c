// isinf check: mfc1 $v1,$f12; daddu $v0,$v1,$zero; lui $a0,0x7FFF; ori $a0,$a0,0xFFFF;
// lui $v1,0x7F80; and $v0,$v0,$a0; subu $v0,$v1,$v0; jr $ra; srl $v0,$v0,31
asm int func_0011E080(float a0) {
    mfc1 $3, $f12
    daddu $2, $3, $zero
    lui $4, 0x7FFF
    ori $4, $4, 0xFFFF
    .word 0x3C037F80
    and $2, $2, $4
    subu $2, $3, $2
    jr $ra
    srl $2, $2, 31
}
