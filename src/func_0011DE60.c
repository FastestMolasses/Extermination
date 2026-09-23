// copysign(float a0, float a1): bits(a0) & 0x7FFFFFFF | bits(a1) & 0x80000000,
// computed on the raw bits in integer registers and moved back to the float
// return register; the return delay slot is empty.
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
