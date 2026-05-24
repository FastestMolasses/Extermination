// All-word: everything as .word except jal/j-external (for R_MIPS_26 relocs)
extern void func_00150260(int, int, int, int);
extern void func_001504D0(int, int, int, int);
extern void func_00150600(int, int, int, int);
extern void func_00150850(int, int, int, int);
extern void func_00150900(int, int, int, int);
extern void func_001513B0(int, int, int, int);
extern void func_001B17A0(int, int, int, int);
extern void func_001C64F0(int, int, int, int);
extern void func_001C68C0(int, int, int, int);

asm void func_00150190(void) {
    .word 0x27bdffd0
    .word 0x7fbf0020
    .word 0x7fb10010
    .word 0x7fb00000
    .word 0x90830005
    .word 0x24020002
    .word 0x70808e28
    .word 0x10620013
    .word 0x70a08628
    .word 0x24020001
    .word 0x1062000c
    .word 0x00000000
    .word 0x10600003
    .word 0x00000000
    .word 0x1000000f
    .word 0x72202628
    jal       func_00150260
    .word 0x00000000
    .word 0x72202628
    jal       func_001513B0
    .word 0x72002e28
    .word 0x10000007
    .word 0x00000000
    jal       func_001504D0
    .word 0x00000000
    .word 0x10000003
    .word 0x00000000
    jal       func_00150600
    .word 0x00000000
    .word 0x72202628
    jal       func_00150900
    .word 0x72002e28
    .word 0xc60c0020
    jal       func_001C64F0
    .word 0x72202628
    .word 0xae020000
    jal       func_00150850
    .word 0x72202628
    jal       func_001C68C0
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
