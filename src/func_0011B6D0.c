// Returns the VIF0 ERR register, *(volatile u32 *)0x10003820 (load in the return delay slot).
asm int func_0011B6D0(void) {
    lui $3, 0x1000
    ori $3, $3, 0x3820
    jr $ra
    lw $2, 0x0($3)
}
