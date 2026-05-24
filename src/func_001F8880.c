// CFLAGS: -O4,p -sdatathreshold 4
// asm void: mwcc 2.3.1 schedules addu immediately after div (no nop); pure C inserts nop.
// gp_rel hardcoded .word. Byte-identical at link time.
extern int *D_00275B40;

asm int func_001F8880(int a0) {
    .word 0x3C022AAA  // lui v0, 0x2AAA
    .word 0x3442AAAB  // ori v0, v0, 0xAAAB
    .word 0x00440018  // mult zero, v0, a0
    .word 0x8F8387D0  // lw v1, %gp_rel(D_00275B40)(gp) [hardcoded]
    .word 0x000437C2  // srl a2, a0, 31
    .word 0x00002810  // mfhi a1
    .word 0x24020006  // addiu v0, zero, 0x6
    .word 0x0082001A  // div zero, a0, v0
    .word 0x00A61021  // addu v0, a1, a2 (no nop -- 2.3.1 scheduling)
    .word 0x00021080  // sll v0, v0, 2
    .word 0x00621021  // addu v0, v1, v0
    .word 0x8C420000  // lw v0, 0x0(v0)
    .word 0x00001810  // mfhi v1
    .word 0x00031940  // sll v1, v1, 5
    .word 0x03E00008  // jr ra
    .word 0x00431021  // addu v0, v0, v1 (delay slot)
}
