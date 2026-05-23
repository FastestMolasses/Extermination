// fabs(float): mfc1 $v1,$f12; lui $v0,0x7FFF; ori $v0,$v0,0xFFFF; and $v1,$v1,$v0; mtc1 $v1,$f0; jr $ra; nop
asm float func_0011DF78(float a0) {
    mfc1 $3, $f12
    lui $2, 0x7FFF
    ori $2, $2, 0xFFFF
    and $3, $3, $2
    mtc1 $3, $f0
    jr $ra
    nop
}
