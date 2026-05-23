// dsll32 $v0,$a0,0; dsra32 $v0,$v0,0; dsra32 $a0,$a0,0; lui $v1,0x7fff; negu $a1,$v0;
// ori $v1,$v1,0xffff; or $v0,$v0,$a1; and $a0,$a0,$v1; srl $v0,$v0,31; lui $v1,0x7ff0;
// or $a0,$a0,$v0; subu $a0,$v1,$a0; jr $ra; srl $v0,$a0,31
asm int func_0011DB58(int a0) {
    dsll32 $2, $4, 0
    dsra32 $2, $2, 0
    dsra32 $4, $4, 0
    lui $3, 0x7FFF
    negu $5, $2
    ori $3, $3, 0xFFFF
    or $2, $2, $5
    and $4, $4, $3
    srl $2, $2, 31
    lui $3, 0x7FF0
    or $4, $4, $2
    subu $4, $3, $4
    jr $ra
    srl $2, $4, 31
}
