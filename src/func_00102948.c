// Copies one 128-bit quadword: *(u128 *)a0 = *(u128 *)a1; the store sits in the return delay slot.
asm void func_00102948(void *a0, void *a1) {
    lq $6, 0x0($5)
    jr $ra
    sq $6, 0x0($4)
}
