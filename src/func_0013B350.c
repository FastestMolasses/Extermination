// All-word: everything as .word except jal/j-external (for R_MIPS_26 relocs)
extern void func_00131ED0(int, int, int, int);
extern void func_0013B420(int, int, int, int);
extern void func_0013B5B0(int, int, int, int);
extern void func_0013B790(int, int, int, int);
extern void func_0013BE60(int, int, int, int);
extern void func_0013BF20(int, int, int, int);
extern void func_001B17A0(int, int, int, int);
extern void func_001C64F0(int, int, int, int);
extern void func_001C6910(int, int, int, int);

asm void func_0013B350(void) {
    .word 0x27bdffd0
    .word 0x7fbf0020
    .word 0x7fb10010
    .word 0x7fb00000
    .word 0x90830005
    .word 0x24020002
    .word 0x70808e28
    .word 0x10620010
    .word 0x70a08628
    .word 0x24020001
    .word 0x10620009
    .word 0x00000000
    .word 0x10600003
    .word 0x00000000
    .word 0x1000000c
    .word 0x72202628
    jal       func_0013B420
    .word 0x00000000
    .word 0x10000007
    .word 0x00000000
    jal       func_0013B5B0
    .word 0x00000000
    .word 0x10000003
    .word 0x00000000
    jal       func_0013B790
    .word 0x00000000
    .word 0x72202628
    jal       func_0013BF20
    .word 0x72002e28
    .word 0x72202628
    jal       func_0013BE60
    .word 0x72002e28
    .word 0x3c023f80
    .word 0x44826000
    jal       func_001C64F0
    .word 0x72202628
    .word 0xae020070
    jal       func_00131ED0
    .word 0x72202628
    jal       func_001C6910
    .word 0x72202628
    jal       func_001B17A0
    .word 0x72202628
    .word 0x8e22004c
    .word 0x0040f809
    .word 0x72202628
    .word 0x7bbf0020
    .word 0x7bb10010
    .word 0x7bb00000
    .word 0x03e00008
    .word 0x27bd0030
}
