// Returns bit 2 of the GIF STAT register: (*(volatile u32 *)0x10003020 & 4) >> 2.
asm int func_0011B3F8(void) {
    lui $3, 0x1000
    ori $3, $3, 0x3020
    lw $2, 0x0($3)
    andi $2, $2, 0x4
    jr $ra
    srl $2, $2, 2
}
