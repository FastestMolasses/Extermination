// Multiple-exit HW register decode: lui $v0,0x1000; ori $v0,$v0,0x3800; lw $a0,0x0($v0);
// andi $v1,$a0,0x3; bnez $v1,.L; andi $v0,$a0,0x700 (delay); jr $ra; daddu $v0,$zero,$zero (delay)
// .L: beqz $v0,.L2; andi $v0,$a0,0xC (delay); jr $ra; addiu $v0,$zero,1 (delay)
// .L2: beqz $v0,.L3; andi $a0,$a0,0x3000 (delay); jr $ra; addiu $v0,$zero,2 (delay)
// .L3: addiu $v1,$zero,4; addiu $v0,$zero,3; jr $ra; movz $v0,$v1,$a0 (delay)
asm int func_0011B6E0(void) {
    lui $2, 0x1000
    ori $2, $2, 0x3800
    lw $4, 0x0($2)
    andi $3, $4, 0x3
    .word 0x14600003
    andi $2, $4, 0x700
    .word 0x03E00008
    daddu $2, $zero, $zero
    .word 0x10400003
    andi $2, $4, 0xC
    .word 0x03E00008
    addiu $2, $zero, 0x1
    .word 0x10400003
    andi $4, $4, 0x3000
    .word 0x03E00008
    addiu $2, $zero, 0x2
    addiu $3, $zero, 0x4
    addiu $2, $zero, 0x3
    jr $ra
    .word 0x0064100A
}
