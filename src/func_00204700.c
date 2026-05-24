// Hybrid-strict: MMI+lui-literal as .word, jal with extern decls
extern void CreateSema(int, int, int, int);
extern void SignalSema(int, int, int, int);
extern void func_002040E0(int, int, int, int);
extern void func_00204140(int, int, int, int);

asm void func_00204700(void) {
    addiu      $sp, $sp, -0x20
    .word 0x7fbf0010
    .word 0x7fb00000
    .word 0x70808628
    jal        SignalSema
    .word 0x8c840040
    addiu      $a0, $zero, 0x5
    jal        func_00204140
    .word 0xae000044
    .word 0x3c011001
    .word 0x8c22b410
    .word 0xae02001c
    .word 0x3c011001
    .word 0x8c22b430
    .word 0xae020020
    .word 0x3c011001
    .word 0x8c22b420
    .word 0xae020024
    .word 0x3c011001
    .word 0x8c22b400
    .word 0xae020028
    .word 0x3c011000
    .word 0x8c222010
    andi       $v0, $v0, 0xF0
    nop
    nop
    .word 0x1440fffa
    nop
    jal        func_002040E0
    .word 0x70002628
    .word 0x3c011001
    .word 0x8c22b010
    .word 0xae02002c
    .word 0x3c011001
    .word 0x8c22b020
    .word 0xae020030
    .word 0x3c011001
    .word 0x8c22b000
    .word 0xae020034
    .word 0x3c011000
    .word 0x8c222020
    .word 0xae020038
    .word 0x3c011000
    .word 0x8c222010
    .word 0xae02003c
    jal        CreateSema
    .word 0x8e040040
    .word 0x7bbf0010
    .word 0x7bb00000
    addiu      $v0, $zero, 0x1
    jr         $ra
    addiu     $sp, $sp, 0x20
}
