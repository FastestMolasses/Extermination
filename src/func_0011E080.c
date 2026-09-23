// func_0011E080: returns 1 when the float's magnitude bits (bits & 0x7FFFFFFF)
// exceed 0x7F800000, i.e. the value is a NaN; else 0.
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
