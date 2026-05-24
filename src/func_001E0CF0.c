// CFLAGS: -O4,p -sdatathreshold 4
// asm void required: pure C gives 87.5% (mwcc hoists addiu a0,0x21 into beqz delay slot).
// The 4 lw gp_rel loads are hardcoded .word (no R_MIPS_GPREL16 reloc) -- 99.375% objdiff,
// but byte-identical at link time (linker computes same -0x7D00 offset from the reloc).
extern int func_001E0CC0(void);
extern int func_001D2910(int);
extern int func_001E1E60(int, int);
extern int func_001E1AD0(int, int);
extern int *D_00275670;

asm void func_001E0CF0(void) {
    .word 0x27BDFFF0  // addiu sp, sp, -0x10
    .word 0x7FBF0000  // sq ra, 0x0(sp)
    jal func_001E0CC0
    .word 0x00000000  // nop
    jal func_001D2910
    .word 0x24040020  // addiu a0, zero, 0x20 (delay slot)
    .word 0x10400016  // beqz v0, +0x16*4 = +0x58
    .word 0x00000000  // nop
    jal func_001D2910
    .word 0x24040021  // addiu a0, zero, 0x21 (delay slot)
    .word 0x10400008  // beqz v0, +0x8*4 = +0x20
    .word 0x24040022  // addiu a0, zero, 0x22 (delay slot)
    .word 0x8F828300  // lw v0, %gp_rel(D_00275670)(gp)
    .word 0x24050003  // addiu a1, zero, 0x3
    jal func_001E1E60
    .word 0x24440180  // addiu a0, v0, 0x180 (delay slot)
    .word 0x8F838300  // lw v1, %gp_rel(D_00275670)(gp)
    .word 0xAC6201D8  // sw v0, 0x1D8(v1)
    .word 0x24040022  // addiu a0, zero, 0x22
    jal func_001D2910
    .word 0x00000000  // nop
    .word 0x10400007  // beqz v0, +0x7*4 = +0x1C
    .word 0x00000000  // nop
    .word 0x8F828300  // lw v0, %gp_rel(D_00275670)(gp)
    .word 0x24050003  // addiu a1, zero, 0x3
    jal func_001E1AD0
    .word 0x244401E0  // addiu a0, v0, 0x1E0 (delay slot)
    .word 0x8F838300  // lw v1, %gp_rel(D_00275670)(gp)
    .word 0xAC6201E8  // sw v0, 0x1E8(v1)
    .word 0x7BBF0000  // lq ra, 0x0(sp)
    .word 0x03E00008  // jr ra
    .word 0x27BD0010  // addiu sp, sp, 0x10
}
