// Copy 4x 128-bit quadwords from a1 to a0
asm void func_00102958(void *a0, void *a1) {
    lq $6, 0x0($5)
    lq $7, 0x10($5)
    lq $8, 0x20($5)
    lq $9, 0x30($5)
    sq $6, 0x0($4)
    sq $7, 0x10($4)
    sq $8, 0x20($4)
    jr $ra
    sq $9, 0x30($4)
}
