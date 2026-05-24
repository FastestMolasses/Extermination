// CFLAGS: -O4,p -sdatathreshold 0
// asm void: lui/addiu for D_002DF788 hardcoded .word (mwcc %hi/%lo not in inline asm);
// sd in non-standard arg positions; byte-identical at link time.
extern void func_00204D60(void *, void *);
extern char D_002DF788[];

asm void func_00206BA0(void *a0, long long a1, long long a2, int a3, int a4) {
    .word 0x27BDFFD0  // addiu sp, sp, -0x30
    .word 0x7FBF0000  // sq ra, 0x0(sp)
    .word 0xFFA50010  // sd a1, 0x10(sp)
    .word 0xFFA60018  // sd a2, 0x18(sp)
    .word 0x8C830048  // lw v1, 0x48(a0)
    .word 0x3C02002E  // lui v0, %hi(D_002DF788) [hardcoded]
    .word 0x27A50010  // addiu a1, sp, 0x10
    .word 0x00E31823  // subu v1, a3, v1
    .word 0xAFA30020  // sw v1, 0x20(sp)
    .word 0x2444F788  // addiu a0, v0, %lo(D_002DF788) [hardcoded]
    jal func_00204D60
    .word 0xAFA80024  // sw t0, 0x24(sp) (delay slot)
    .word 0x7BBF0000  // lq ra, 0x0(sp)
    .word 0x03E00008  // jr ra
    .word 0x27BD0030  // addiu sp, sp, 0x30
}
