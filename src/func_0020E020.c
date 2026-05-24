// CFLAGS: -O4,p -sdatathreshold 4
// asm void: loop body uses paddub a0,zero,zero for counter init; lui/addiu %hi/%lo
// and gp_rel hardcoded .word. Byte-identical at link time.
extern int D_00821300[0x40];
extern int D_00275C90;

asm void func_0020E020(void) {
    .word 0x3C030082  // lui v1, %hi(D_00821300) [hardcoded]
    .word 0x24651300  // addiu a1, v1, %lo(D_00821300) [hardcoded -- a1 = &D_00821300]
    .word 0x70002628  // paddub a0, zero, zero  [a0 = 0, loop counter]
    // loop:
    .word 0xACA00000  // sw zero, 0x0(a1)
    .word 0xACA00004  // sw zero, 0x4(a1)
    .word 0x24840001  // addiu a0, a0, 0x1    [i++]
    .word 0xACA00008  // sw zero, 0x8(a1)
    .word 0xACA0000C  // sw zero, 0xC(a1)
    .word 0x28830010  // slti v1, a0, 0x10    [v1 = (i < 16)]
    .word 0x1460FFF9  // bnez v1, loop (offset -7)
    .word 0x24A50010  // addiu a1, a1, 0x10   [p += 0x10 (delay)]
    .word 0x03E00008  // jr ra
    .word 0xAF808920  // sw zero, %gp_rel(D_00275C90)(gp) [hardcoded, delay]
}
