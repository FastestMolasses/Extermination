// CFLAGS: -O4,p -sdatathreshold 4
// asm void: instruction scheduling differs (addiu a1 before second jal, addiu a2 order).
// gp_rel and lui/addiu %hi/%lo hardcoded .word. Byte-identical at link time.
extern char *D_00275670;
extern char D_00816440[0x10000];
extern void func_001D4750(int);
extern void func_001D2090(int, char *);
extern char D_00239C90[8];

asm void func_001D4960(void) {
    .word 0x27BDFFF0  // addiu sp, sp, -0x10
    .word 0x7FBF0000  // sq ra, 0x0(sp)
    jal func_001D4750
    .word 0x70002628  // paddub a0, zero, zero (delay slot)
    .word 0x3C020024  // lui v0, %hi(D_00239C90) [hardcoded]
    .word 0x24459C90  // addiu a1, v0, %lo(D_00239C90) [hardcoded]
    jal func_001D2090
    .word 0x70002628  // paddub a0, zero, zero (delay slot)
    .word 0x8F888300  // lw t0, %gp_rel(D_00275670)(gp) [hardcoded]
    .word 0x3C030081  // lui v1, %hi(D_00816440) [hardcoded]
    .word 0x24666440  // addiu a2, v1, %lo(D_00816440) [hardcoded]
    .word 0x24050030  // addiu a1, zero, 0x30
    .word 0x8D07009C  // lw a3, 0x9C(t0)
    .word 0x8D030010  // lw v1, 0x10(t0)
    .word 0x24040008  // addiu a0, zero, 0x8
    .word 0x000739C0  // sll a3, a3, 7
    .word 0xA0650003  // sb a1, 0x3(v1)
    .word 0x8D030010  // lw v1, 0x10(t0)
    .word 0x00C72821  // addu a1, a2, a3
    .word 0xAC650004  // sw a1, 0x4(v1)
    .word 0x8D030010  // lw v1, 0x10(t0)
    .word 0xA4640000  // sh a0, 0x0(v1)
    .word 0x8D030010  // lw v1, 0x10(t0)
    .word 0x24630010  // addiu v1, v1, 0x10
    .word 0xAD030010  // sw v1, 0x10(t0)
    .word 0x7BBF0000  // lq ra, 0x0(sp)
    .word 0x03E00008  // jr ra
    .word 0x27BD0010  // addiu sp, sp, 0x10 (delay slot)
}
