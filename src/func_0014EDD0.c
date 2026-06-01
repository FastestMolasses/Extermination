// All-word: everything as .word except jal/j-external (for R_MIPS_26 relocs)
extern void func_0014EEF0(int, int, int, int);
extern void func_0014F390(int, int, int, int);
extern void func_0014FBC0(int, int, int, int);
extern void func_00150850(int, int, int, int);
extern void func_00150900(int, int, int, int);
extern void func_001509A0(int, int, int, int);
extern void func_001513B0(int, int, int, int);
extern void func_001B17A0(int, int, int, int);
extern void func_001B4810(int, int, int, int);
extern void anim_advance_time(int, int, int, int);
extern void func_001C68C0(int, int, int, int);

asm void func_0014EDD0(void) {
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
    jal       func_0014EEF0
    .word 0x00000000
    .word 0x10000007
    .word 0x00000000
    jal       func_0014F390
    .word 0x00000000
    .word 0x10000003
    .word 0x00000000
    jal       func_0014FBC0
    .word 0x00000000
    .word 0x72202628
    jal       func_001B4810
    .word 0x00000000
    .word 0x72202628
    jal       func_00150900
    .word 0x72002e28
    .word 0x72202628
    jal       func_001509A0
    .word 0x72002e28
    .word 0x72202628
    jal       func_001513B0
    .word 0x72002e28
    .word 0xc62100b4
    .word 0x3c02c2c8
    .word 0x44820000
    .word 0x00000000
    .word 0x46000836
    .word 0x00000000
    .word 0x45000005
    .word 0x24020001
    .word 0x24020002
    .word 0x10000003
    .word 0xa2220000
    .word 0x24020001
    .word 0xa2220000
    .word 0xc60c0020
    jal       anim_advance_time
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
