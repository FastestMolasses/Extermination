// HW register DMA poll + byte write: spins on 0x1000F130 bit 0x8000, then sb to 0x1000F180
asm void func_0010CEE8(int a0) {
    lui $3, 0x1000
    ori $3, $3, 0xF130
    lw $2, 0x0($3)
    andi $2, $2, 0x8000
    nop
    nop
    nop
    .word 0x1440FFFA
    nop
    .word 0x3C031000
    daddu $2, $4, $zero
    ori $3, $3, 0xF180
    jr $ra
    sb $4, 0x0($3)
}
