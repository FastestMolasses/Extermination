// CFLAGS: -O4,p -sdatathreshold 4
// asm void: pure C generates addiu+addiu to reach D_00275C20+0xa0b8;
// original uses lui at,0x1 / addu at,v1,at / lw v1,-0x5f48(at).
// gp_rel hardcoded .word. Byte-identical at link time.
extern char *D_00275C20;

asm void func_001E8B40(int a0) {
    .word 0x24050001  // addiu a1, zero, 0x1
    .word 0x1085000B  // beq a0, a1, case1 (offset +11)
    .word 0x00000000  // nop (delay)
    .word 0x10800003  // beqz a0, case0 (offset +3)
    .word 0x00000000  // nop (delay)
    .word 0x1000000A  // b exit (offset +10)
    .word 0x00000000  // nop (delay)
    // case0:
    .word 0x8F8388B0  // lw v1, %gp_rel(D_00275C20)(gp) [hardcoded]
    .word 0x3C010001  // lui at, 0x1
    .word 0x00610821  // addu at, v1, at
    .word 0x8C23A0B8  // lw v1, -0x5f48(at) [= *(D_00275C20 + 0xa0b8)]
    .word 0x10000004  // b exit (offset +4)
    .word 0xA0650005  // sb a1, 0x5(v1) (delay)
    // case1:
    .word 0x8F8388B0  // lw v1, %gp_rel(D_00275C20)(gp) [hardcoded]
    .word 0x8C630058  // lw v1, 0x58(v1) [= *(D_00275C20 + 0x58)]
    .word 0xA0650005  // sb a1, 0x5(v1)
    // exit:
    .word 0x03E00008  // jr ra
    .word 0x00000000  // nop (delay)
}
