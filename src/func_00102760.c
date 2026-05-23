// VU normalize: load vec from a1, compute length via VU, divide, store to a0
asm void func_00102760(void *a0, void *a1) {
    lqc2 $vf4, 0x0($5)
    .word 0x4BC4216A
    .word 0x4B052941
    .word 0x4B052942
    .word 0x4A0503BD
    .word 0x4A0003BF
    .word 0x4B000160
    .word 0x4A6503BC
    .word 0x4BE001AC
    .word 0x4A0003BF
    .word 0x4BC0219C
    jr $ra
    sqc2 $vf6, 0x0($4)
}
