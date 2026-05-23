extern void func_00207E40(void);

asm void func_00207CD0(void) {
    ld $10, 0x80($4)
    lui $2, (0x80808080 >> 16)
    addiu $6, $0, 0x7AF0
    addiu $7, $0, 0x20
    addiu $4, $0, 0x1
    addiu $8, $0, 0x18
    ori $5, $0, 0x8B10
    j func_00207E40
    ori $9, $2, (0x80808080 & 0xFFFF)
}
