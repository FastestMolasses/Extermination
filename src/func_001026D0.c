/* Handwritten function */
// 4x4 matrix × 4 vectors: load matrix rows vf4-vf7 from a1, loop 4 times
asm void func_001026D0(void *a0, void *a1, void *a2) {
    lqc2 $vf4, 0x0($5)
    lqc2 $vf5, 0x10($5)
    lqc2 $vf6, 0x20($5)
    lqc2 $vf7, 0x30($5)
    addiu $7, $zero, 0x4
L_001026E4:
    lqc2 $vf8, 0x0($6)
    .word 0x4BE821BC
    .word 0x4BE828BD
    .word 0x4BE830BE
    .word 0x4BE83A4B
    sqc2 $vf9, 0x0($4)
    .word 0x20E7FFFF
    .word 0x20C60010
    .word 0x1407FFF7
    .word 0x20840010
    jr $ra
    nop
}
