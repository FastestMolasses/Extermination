// CFLAGS: -O4,p -sdatathreshold 0
// asm void: 2.3.1 dead instruction (paddub a2,v0,zero after b+addiu delay slot) not emitted
// by pure C. lui/addiu hardcoded .word. Byte-identical at link time.
extern char D_002DF740[8];
extern char D_0028F700[8];
extern int func_002034C0(void);
extern void func_00203C90(void);
extern void func_002036E0(char *, char *, int);

asm int func_00203460(void) {
    .word 0x27BDFFF0  // addiu sp, sp, -0x10
    .word 0x7FBF0000  // sq ra, 0x0(sp)
    jal func_002034C0
    .word 0x00000000  // nop (delay slot)
    .word 0x14400006  // bnez v0, +6*4=+0x18
    .word 0x70403628  // paddub a2, v0, zero (delay slot)
    jal func_00203C90
    .word 0x00000000  // nop (delay slot)
    .word 0x1000000A  // b +0xA*4=+0x28
    .word 0x24020001  // addiu v0, zero, 0x1 (delay slot)
    .word 0x70403628  // paddub a2, v0, zero (DEAD -- 2.3.1 dead instruction)
    // .L0020348C:
    .word 0x3C02002E  // lui v0, %hi(D_002DF740) [hardcoded]
    .word 0x2444F740  // addiu a0, v0, %lo(D_002DF740) [hardcoded]
    .word 0x3C020029  // lui v0, %hi(D_0028F700) [hardcoded]
    jal func_002036E0
    .word 0x2445F700  // addiu a1, v0, %lo(D_0028F700) [hardcoded, delay slot]
    jal func_00203C90
    .word 0x00000000  // nop (delay slot)
    .word 0x24020001  // addiu v0, zero, 0x1
    // .L002034AC:
    .word 0x7BBF0000  // lq ra, 0x0(sp)
    .word 0x03E00008  // jr ra
    .word 0x27BD0010  // addiu sp, sp, 0x10 (delay slot)
}
