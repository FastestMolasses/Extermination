// DMA wait loop with HW register accesses using .word to prevent mwcc peephole opts
asm int func_0011B580(void *a0, int a1) {
    daddu $7, $zero, $zero
    .word 0x10A00013
    addiu $3, $5, -0x1
    lui $6, 0x1000
    .word 0x3C051000
    ori $6, $6, 0x3020
    .word 0x3C091000
    ori $5, $5, 0x6000
    addiu $8, $zero, -0x1
    nop
    lw $2, 0x0($6)
    and $2, $2, $9
    .word 0x50400003
    lq $2, 0x0($4)
    .word 0x03E00008
    daddu $2, $7, $zero
    addiu $7, $7, 0x1
    addiu $4, $4, 0x10
    addiu $3, $3, -0x1
    .word 0x1468FFF6
    sq $2, 0x0($5)
    jr $ra
    daddu $2, $7, $zero
}
