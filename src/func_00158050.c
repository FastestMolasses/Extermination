// CFLAGS: -O4,p -sdatathreshold 0
// asm void: lui v1/%hi(D_008105E0) interleaved before lw v0 (not after lui a2 as pure C);
// All lui/addiu %hi/%lo hardcoded .word. Byte-identical at link time.
extern char D_700038A0[0x10];
extern char D_008105E0[0x100];
extern int func_00102948(char *, char *, int);

asm int func_00158050(int a0) {
    .word 0x27BDFFF0  // addiu sp, sp, -0x10
    .word 0x7FBF0000  // sq ra, 0x0(sp)
    .word 0x8C84001C  // lw a0, 0x1C(a0)
    .word 0x3C027000  // lui v0, %hi(D_700038A0) [hardcoded]
    .word 0x244538A0  // addiu a1, v0, %lo(D_700038A0) [hardcoded]
    .word 0x3C017000  // lui at, 0x7000
    .word 0x3C030081  // lui v1, %hi(D_008105E0) [hardcoded -- interleaved here]
    .word 0x8C82001C  // lw v0, 0x1C(a0)
    .word 0x3C063F80  // lui a2, 0x3F80
    .word 0xC44000B0  // lwc1 f0, 0xB0(v0)
    .word 0x246405E0  // addiu a0, v1, %lo(D_008105E0) [hardcoded]
    .word 0xE42038A0  // swc1 f0, 0x38A0(at)
    .word 0xC44000B4  // lwc1 f0, 0xB4(v0)
    .word 0x3C017000  // lui at, 0x7000
    .word 0xE42038A4  // swc1 f0, 0x38A4(at)
    .word 0xC44000B8  // lwc1 f0, 0xB8(v0)
    .word 0x3C017000  // lui at, 0x7000
    .word 0xE42038A8  // swc1 f0, 0x38A8(at)
    .word 0x3C017000  // lui at, 0x7000
    jal func_00102948
    .word 0xAC2638AC  // sw a2, 0x38AC(at) (delay slot)
    .word 0x7BBF0000  // lq ra, 0x0(sp)
    .word 0x24020001  // addiu v0, zero, 0x1
    .word 0x03E00008  // jr ra
    .word 0x27BD0010  // addiu sp, sp, 0x10 (delay slot)
}
