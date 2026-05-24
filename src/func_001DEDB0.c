// CFLAGS: -O4,p -sdatathreshold 4
// asm void: beq delay slot has dead addiu v0,zero,2 (hoisted but immediately overwritten).
// gp_rel hardcoded .word. Byte-identical at link time.
extern int *D_00275670;

asm int *func_001DEDB0(int a0) {
    .word 0x24020009  // addiu v0, zero, 0x9
    .word 0x10820004  // beq a0, v0, .L001DEDC8 (+4*4=+0x10)
    .word 0x24020002  // addiu v0, zero, 0x2 (delay slot -- dead/overwritten by lw below)
    .word 0x8F828300  // lw v0, %gp_rel(D_00275670)(gp) [hardcoded]
    .word 0x10000003  // b .L001DEDD0 (+3*4=+0xC)
    .word 0x24422470  // addiu v0, v0, 0x2470 (delay slot)
    // .L001DEDC8:
    .word 0x8F828300  // lw v0, %gp_rel(D_00275670)(gp) [hardcoded]
    .word 0x24422490  // addiu v0, v0, 0x2490
    // .L001DEDD0:
    .word 0x03E00008  // jr ra
    .word 0x00000000  // nop (delay slot)
}
