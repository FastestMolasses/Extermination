// 4x4 matrix × vec, perspective divide, optional fixed-point convert
asm void func_00103198(void *a0, void *a1, void *a2, int a3) {
    lqc2 $vf4, 0x0($5)
    lqc2 $vf5, 0x10($5)
    lqc2 $vf6, 0x20($5)
    lqc2 $vf7, 0x30($5)
    lqc2 $vf8, 0x0($6)
    .word 0x4BE821BC
    .word 0x4BE828BD
    .word 0x4BE830BE
    .word 0x4BE83A4B
    .word 0x4BE903BC
    .word 0x4A0003BF
    .word 0x4BC04A5C
    .word 0x10E00002
    .word 0x4BEA497D
    .word 0x4A6A497C
    jr $ra
    sqc2 $vf10, 0x0($4)
}
