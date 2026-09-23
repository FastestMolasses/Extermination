// func_0011DB58: NaN test on a double held in one 64-bit register. With hi/lo
// its two words: returns 1 when ((hi & 0x7FFFFFFF) | (lo != 0)) > 0x7FF00000
// (unsigned), else 0.
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
