/* Handwritten function */
// 4x4 matrix × N vectors with perspective divide loop, optional vftoi
asm void func_00103140(void *a0, void *a1, void *a2, int a3, int a4) {
    lqc2 $vf4, 0x0($5)
    lqc2 $vf5, 0x10($5)
    lqc2 $vf6, 0x20($5)
    lqc2 $vf7, 0x30($5)
L_00103150:
    lqc2 $vf8, 0x0($6)
    .word 0x4BE821BC
    .word 0x4BE828BD
    .word 0x4BE830BE
    .word 0x4BE83A4B
    .word 0x4BE903BC
    .word 0x4A0003BF
    .word 0x4BC04A5C
    .word 0x11000002
    .word 0x4BEA497D
    .word 0x4A6A497C
    sqc2 $vf10, 0x0($4)
    .word 0x20E7FFFF
    .word 0x20C60010
    .word 0x1407FFF1
    .word 0x20840010
    jr $ra
    nop
}
