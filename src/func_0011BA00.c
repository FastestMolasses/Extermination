// Returns the VIF1 ERR register, *(volatile u32 *)0x10003C20 (load in the return delay slot).
asm int func_0011BA00(void) {
    lui $3, 0x1000
    ori $3, $3, 0x3C20
    jr $ra
    lw $2, 0x0($3)
}
