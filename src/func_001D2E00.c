extern int *D_00275670;

asm int func_001D2E00(int a0) {
    lw $3, D_00275670($28)
    sll $2, $4, 2
    addu $2, $2, $3
    jr $31
    lw $2, 0x2520($2)
}
