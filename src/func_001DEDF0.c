// CFLAGS: -O4,p -sdatathreshold 0
// asm void: paddub v1,a0,zero saves a0 before jal overwrites it; sw v1 in jal delay slot.
// lui/addiu %hi/%lo hardcoded .word. Byte-identical at link time.
extern char *func_001DEDB0(int);
extern void func_001DEE80(int, int);
extern void func_001DEEC0(int, int);
extern int D_0026E850;

asm void func_001DEDF0(void) {
    .word 0x27BDFFF0  // addiu sp, sp, -0x10
    .word 0x7FBF0000  // sq ra, 0x0(sp)
    jal func_001DEDB0
    .word 0x24040002  // addiu a0, zero, 0x2 (delay slot)
    .word 0xA0400000  // sb zero, 0x0(v0)
    .word 0xA0400003  // sb zero, 0x3(v0)
    .word 0xA0400002  // sb zero, 0x2(v0)
    .word 0x70801E28  // paddub v1, a0, zero (save a0=2 into v1)
    .word 0xA0400001  // sb zero, 0x1(v0)
    .word 0x24040009  // addiu a0, zero, 0x9
    jal func_001DEDB0
    .word 0xAC430008  // sw v1, 0x8(v0) (delay slot -- v1=2)
    .word 0xA0400000  // sb zero, 0x0(v0)
    .word 0xA0400003  // sb zero, 0x3(v0)
    .word 0xA0400002  // sb zero, 0x2(v0)
    .word 0x70801E28  // paddub v1, a0, zero (save a0=9 into v1)
    .word 0xA0400001  // sb zero, 0x1(v0)
    .word 0xAC430008  // sw v1, 0x8(v0) (v1=9)
    .word 0x3C020027  // lui v0, %hi(D_0026E850) [hardcoded]
    .word 0x24040002  // addiu a0, zero, 0x2
    jal func_001DEE80
    .word 0x2445E850  // addiu a1, v0, %lo(D_0026E850) [hardcoded, delay slot]
    .word 0x24040002  // addiu a0, zero, 0x2
    jal func_001DEEC0
    .word 0x24050060  // addiu a1, zero, 0x60 (delay slot)
    .word 0x3C020027  // lui v0, %hi(D_0026E850) [hardcoded]
    .word 0x24040009  // addiu a0, zero, 0x9
    jal func_001DEE80
    .word 0x2445E850  // addiu a1, v0, %lo(D_0026E850) [hardcoded, delay slot]
    .word 0x24040009  // addiu a0, zero, 0x9
    jal func_001DEEC0
    .word 0x24050060  // addiu a1, zero, 0x60 (delay slot)
    .word 0x7BBF0000  // lq ra, 0x0(sp)
    .word 0x03E00008  // jr ra
    .word 0x27BD0010  // addiu sp, sp, 0x10 (delay slot)
}
