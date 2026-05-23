extern void func_00207E40(void);

asm void func_00207CA0(void) {
    ld $10, 0x78($4)
    lui $2, (0x80808080 >> 16)
    addiu $6, $0, 0x7B30
    addiu $7, $0, 0x8
    addiu $4, $0, 0x1
    addiu $8, $0, 0x10
    ori $5, $0, 0x8AE0
    j func_00207E40
    ori $9, $2, (0x80808080 & 0xFFFF)
}
