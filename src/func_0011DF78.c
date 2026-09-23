// fabs(float): returns the argument's bit pattern & 0x7FFFFFFF (sign bit cleared).
asm float func_0011DF78(float a0) {
    mfc1 $3, $f12
    lui $2, 0x7FFF
    ori $2, $2, 0xFFFF
    and $3, $3, $2
    mtc1 $3, $f0
    jr $ra
    nop
}
