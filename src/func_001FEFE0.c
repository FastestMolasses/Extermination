// CFLAGS: -O4,p -sdatathreshold 4
// asm void: mwcc 2.3.1 schedules sb D_00275BD8 after addiu a0,0x2 (pure C puts it before).
// lui/addiu and gp_rel hardcoded .word (no %hi/%lo in mwcc inline asm). Byte-identical at link time.
extern unsigned char D_00810700[8];
extern unsigned char D_00810730[0x100];
extern unsigned char D_00810701[8];
extern unsigned char D_00275BD8;
extern void func_001FF080(int, int);

asm void func_001FEFE0(int a0) {
    .word 0x308200FF  // andi v0, a0, 0xFF
    .word 0x30420080  // andi v0, v0, 0x80
    .word 0x10400008  // beqz v0, .L001FF00C (+8*4=+0x20)
    .word 0x3082007F  // andi v0, a0, 0x7F (delay slot)
    .word 0x3C010081  // lui at, %hi(D_00810700) [hardcoded]
    .word 0x90230700  // lbu v1, %lo(D_00810700)(at) [hardcoded]
    .word 0x3C020081  // lui v0, %hi(D_00810730) [hardcoded]
    .word 0x24420730  // addiu v0, v0, %lo(D_00810730) [hardcoded]
    .word 0x00431021  // addu v0, v0, v1
    .word 0xA0440000  // sb a0, 0x0(v0)
    .word 0x3082007F  // andi v0, a0, 0x7F
    // .L001FF00C:
    .word 0x3C010081  // lui at, %hi(D_00810701) [hardcoded]
    .word 0xA0220701  // sb v0, %lo(D_00810701)(at) [hardcoded]
    .word 0x24020001  // addiu v0, zero, 0x1
    .word 0x24040002  // addiu a0, zero, 0x2
    .word 0xA3828868  // sb v0, %gp_rel(D_00275BD8)(gp) [hardcoded]
    j func_001FF080
    .word 0x70002E28  // paddub a1, zero, zero (delay slot)
}
