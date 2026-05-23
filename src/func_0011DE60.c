// copysign(float,float): mfc1 $a1,$f12; mfc1 $a0,$f13; daddu $v1,$a0,$zero;
// lui $v0,0x7FFF; ori $v0,$v0,0xFFFF; lui $a0,0x8000; and $a1,$a1,$v0;
// and $v1,$v1,$a0; or $a1,$a1,$v1; mtc1 $a1,$f0; jr $ra; nop
asm float func_0011DE60(float a0, float a1) {
    mfc1 $5, $f12
    mfc1 $4, $f13
    daddu $3, $4, $zero
    lui $2, 0x7FFF
    ori $2, $2, 0xFFFF
    .word 0x3C048000
    and $5, $5, $2
    and $3, $3, $4
    or $5, $5, $3
    mtc1 $5, $f0
    jr $ra
    nop
}
