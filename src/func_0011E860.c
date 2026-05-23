// bgez $a0,.L; daddu $v0,$a0,$zero (delay); negu $v0,$v0; .L: jr $ra; nop
// bgez $4,2 = 0x04810002
asm int func_0011E860(int a0) {
    .word 0x04810002
    daddu $2, $4, $zero
    negu $2, $2
    jr $ra
    nop
}
