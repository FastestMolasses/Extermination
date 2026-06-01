// All-word: everything as .word except jal/j-external (for R_MIPS_26 relocs)
extern void func_0011E620(int, int, int, int);
extern void func_0014C490(int, int, int, int);
extern void func_0014C680(int, int, int, int);
extern void func_0014C9D0(int, int, int, int);
extern void func_0014CD30(int, int, int, int);
extern void func_0014CDD0(int, int, int, int);
extern void func_001B1470(int, int, int, int);
extern void func_001B17A0(int, int, int, int);
extern void func_001B5360(int, int, int, int);
extern void anim_advance_time(int, int, int, int);
extern void func_001C68C0(int, int, int, int);

asm void func_0014C3A0(void) {
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
    .word 0xc60c0024
    jal       func_0014C680
    .word 0x00000000
    .word 0x10000007
    .word 0x00000000
    jal       func_0014C9D0
    .word 0x00000000
    .word 0x10000003
    .word 0x00000000
    jal       func_0014C490
    .word 0x00000000
    .word 0xc60c0024
    jal       func_0011E620
    .word 0xc60d002c
    .word 0x3c02bf80
    .word 0x44820800
    jal       func_001B1470
    .word 0x46000b02
    .word 0xe62000c0
    jal       func_001B5360
    .word 0x72202628
    .word 0x72202628
    jal       func_0014CD30
    .word 0x72002e28
    .word 0x72202628
    jal       func_0014CDD0
    .word 0x72002e28
    .word 0xc60c0020
    jal       anim_advance_time
    .word 0x72202628
    .word 0xae020000
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
