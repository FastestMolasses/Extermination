// Copy 3 rows lq from a1 to a0, add vf4 from a2 to vf5 from a1 row 3
asm void func_00102918(void *a0, void *a1, void *a2) {
    lqc2 $vf4, 0x0($6)
    lqc2 $vf5, 0x30($5)
    lq $7, 0x0($5)
    lq $8, 0x10($5)
    lq $9, 0x20($5)
    .word 0x4BC42968
    sq $7, 0x0($4)
    sq $8, 0x10($4)
    sq $9, 0x20($4)
    jr $ra
    sqc2 $vf5, 0x30($4)
}
