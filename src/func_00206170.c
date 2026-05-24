// CFLAGS: -O4,p -sdatathreshold 4
// asm void: mwcc fills beqz delay slot causing branch offset mismatch; gp_rel and hi/lo
// hardcoded .word (mwcc inline asm rejects %hi/%lo). Byte-identical at link time.
extern int D_00275C84;
extern char D_007A55A0[8];
extern void func_00206010(char *);

asm int func_00206170(void) {
    .word 0x27BDFFF0  // addiu sp, sp, -0x10
    .word 0x7FBF0000  // sq ra, 0x0(sp)
    .word 0x8F828914  // lw v0, %gp_rel(D_00275C84)(gp) [hardcoded]
    .word 0x10400005  // beqz v0, +5*4=+0x14
    .word 0x00000000  // nop (delay slot)
    .word 0x3C02007A  // lui v0, %hi(D_007A55A0) [hardcoded]
    jal func_00206010
    .word 0x244455A0  // addiu a0, v0, %lo(D_007A55A0) [hardcoded, delay slot]
    .word 0xAF808914  // sw zero, %gp_rel(D_00275C84)(gp) [hardcoded]
    // .L00206194:
    .word 0x0000000F  // sync
    .word 0x42000038  // ei
    .word 0x7BBF0000  // lq ra, 0x0(sp)
    .word 0x70001628  // paddub v0, zero, zero
    .word 0x03E00008  // jr ra
    .word 0x27BD0010  // addiu sp, sp, 0x10
}
