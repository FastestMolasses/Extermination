// CFLAGS: -O4,p -sdatathreshold 4
// asm void: 2.3.1 dead instruction (paddub v0,zero,zero after b+lw delay) not emitted by
// pure C. gp_rel hardcoded .word. Byte-identical at link time.
extern short D_00275BCC;
extern int *D_00275BD0;

asm int func_001AF780(void) {
    .word 0x8782885C  // lh v0, %gp_rel(D_00275BCC)(gp) [hardcoded]
    .word 0x2841001F  // slti at, v0, 0x1F
    .word 0x54200009  // bnel at, zero, .L001AF7B0 (+9*4=+0x24)
    .word 0x70001628  // paddub v0, zero, zero (delay slot)
    .word 0x8F838860  // lw v1, %gp_rel(D_00275BD0)(gp) [hardcoded]
    .word 0x2442FFFF  // addiu v0, v0, -0x1
    .word 0xA782885C  // sh v0, %gp_rel(D_00275BCC)(gp) [hardcoded]
    .word 0x24620004  // addiu v0, v1, 0x4
    .word 0xAF828860  // sw v0, %gp_rel(D_00275BD0)(gp) [hardcoded]
    .word 0x10000002  // b .L001AF7B0 (+2*4=+0x8)
    .word 0x8C620000  // lw v0, 0x0(v1) (delay slot)
    .word 0x70001628  // paddub v0, zero, zero (DEAD -- 2.3.1 dead instruction)
    // .L001AF7B0:
    .word 0x03E00008  // jr ra
    .word 0x00000000  // nop (delay slot)
}
