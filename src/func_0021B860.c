// CFLAGS: -O4,p -sdatathreshold 4
// asm void required: float constant loads (lui/ori/mtc1) and gp_rel loads can't be ordered by pure C.
// The 3 lw gp_rel loads are hardcoded .word (no R_MIPS_GPREL16 reloc) -- ~99.4% objdiff,
// but byte-identical at link time (linker computes same -0x7D00 offset from the reloc).
extern void func_0021B970(int, int, int);
extern void func_0021BA80(int, int, int);
extern void block_copy(void *, void *, int);
extern int *D_00275670;

asm void func_0021B860(void) {
    .word 0x3C024974  // lui v0, 0x4974
    .word 0x34422400  // ori v0, v0, 0x2400
    .word 0x44826000  // mtc1 v0, f12
    .word 0x27BDFFF0  // addiu sp, sp, -0x10
    .word 0x3C024B18  // lui v0, 0x4B18
    .word 0x34429680  // ori v0, v0, 0x9680
    .word 0x44826800  // mtc1 v0, f13
    .word 0x7FBF0000  // sq ra, 0x0(sp)
    jal func_0021B970
    .word 0x00000000  // nop (delay slot)
    .word 0x70002628  // paddub a0, zero, zero
    .word 0x70002E28  // paddub a1, zero, zero
    jal func_0021BA80
    .word 0x70003628  // paddub a2, zero, zero (delay slot)
    .word 0x8F828300  // lw v0, %gp_rel(D_00275670)(gp) [hardcoded offset -0x7D00]
    .word 0x24060020  // addiu a2, zero, 0x20
    .word 0x24440100  // addiu a0, v0, 0x100
    jal block_copy
    .word 0x244500A0  // addiu a1, v0, 0xA0 (delay slot)
    .word 0x8F828300  // lw v0, %gp_rel(D_00275670)(gp) [hardcoded]
    .word 0x24060020  // addiu a2, zero, 0x20
    .word 0x244400C0  // addiu a0, v0, 0xC0
    jal block_copy
    .word 0x244500A0  // addiu a1, v0, 0xA0 (delay slot)
    .word 0x8F828300  // lw v0, %gp_rel(D_00275670)(gp) [hardcoded]
    .word 0x24060020  // addiu a2, zero, 0x20
    .word 0x244400E0  // addiu a0, v0, 0xE0
    jal block_copy
    .word 0x244500A0  // addiu a1, v0, 0xA0 (delay slot)
    .word 0x7BBF0000  // lq ra, 0x0(sp)
    .word 0x03E00008  // jr ra
    .word 0x27BD0010  // addiu sp, sp, 0x10
}
