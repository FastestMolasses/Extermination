// CFLAGS: -O4,p -sdatathreshold 4
// asm void: mwcc 2.3.1 schedules addiu v1,-1 between lui/sw for D_00264E34 (pure C puts
// sw immediately after lui). All lui/addiu and gp_rel hardcoded .word. Byte-identical at link time.
extern int D_00264E30[2];
extern int D_00264E34[2];
extern int D_00264E38[2];
extern int D_00275C58;
extern int D_00275C5C;
extern int D_00275C60;
extern int D_00275C64;
extern int D_00275C68;
extern int D_00275C6C;
extern int D_00275840;
extern int D_00275844;

asm void func_001FE8D0(void) {
    .word 0x3C010026  // lui at, %hi(D_00264E30) [hardcoded]
    .word 0xAC204E30  // sw zero, %lo(D_00264E30)(at) [hardcoded]
    .word 0x3C010026  // lui at, %hi(D_00264E34) [hardcoded]
    .word 0x2403FFFF  // addiu v1, zero, -0x1 (scheduled before sw for D_00264E34)
    .word 0xAC204E34  // sw zero, %lo(D_00264E34)(at) [hardcoded]
    .word 0x3C010026  // lui at, %hi(D_00264E38) [hardcoded]
    .word 0xAF8088E8  // sw zero, %gp_rel(D_00275C58)(gp) [hardcoded]
    .word 0xAF8088EC  // sw zero, %gp_rel(D_00275C5C)(gp) [hardcoded]
    .word 0xAC204E38  // sw zero, %lo(D_00264E38)(at) [hardcoded]
    .word 0xAF8088F0  // sw zero, %gp_rel(D_00275C60)(gp) [hardcoded]
    .word 0xAF8088F4  // sw zero, %gp_rel(D_00275C64)(gp) [hardcoded]
    .word 0xAF8088F8  // sw zero, %gp_rel(D_00275C68)(gp) [hardcoded]
    .word 0xAF8088FC  // sw zero, %gp_rel(D_00275C6C)(gp) [hardcoded]
    .word 0xAF8384D0  // sw v1, %gp_rel(D_00275840)(gp) [hardcoded]
    .word 0x03E00008  // jr ra
    .word 0xAF8384D4  // sw v1, %gp_rel(D_00275844)(gp) [hardcoded, delay slot]
}
