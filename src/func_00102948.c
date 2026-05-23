// lq $a2,0x0($a1); jr $ra; sq $a2,0x0($a0)
asm void func_00102948(void *a0, void *a1) {
    lq $6, 0x0($5)
    jr $ra
    sq $6, 0x0($4)
}
