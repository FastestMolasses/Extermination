// All-word: everything as .word except jal/j-external (for R_MIPS_26 relocs)
extern void func_001B1B30(int, int, int, int);
extern void func_001C68C0(int, int, int, int);

asm void func_001BC300(void) {
    .word 0x27bdffe0
    .word 0x7fbf0010
    .word 0x7fb00000
    jal       func_001C68C0
    .word 0x70808628
    .word 0xc60100b4
    .word 0x3c024120
    .word 0x44820000
    .word 0xc60c00b0
    .word 0xc60e00b8
    .word 0x72002628
    jal       func_001B1B30
    .word 0x46010340
    .word 0x8e02004c
    .word 0x0040f809
    .word 0x72002628
    .word 0x7bbf0010
    .word 0x7bb00000
    .word 0x03e00008
    .word 0x27bd0020
}
