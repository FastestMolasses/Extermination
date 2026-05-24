// All-word: everything as .word except jal/j-external (for R_MIPS_26 relocs)
extern void func_001AFC10(int, int, int, int);
extern void func_001B0FD0(int, int, int, int);
extern void func_001B17A0(int, int, int, int);
extern void func_001C6380(int, int, int, int);

asm void func_001C4820(void) {
    .word 0x27bdffe0
    .word 0x7fbf0010
    .word 0x7fb00000
    .word 0x90850004
    .word 0x24030003
    .word 0x10a3001a
    .word 0x70808628
    .word 0x24030002
    .word 0x50a30018
    .word 0x72002628
    .word 0x24030001
    .word 0x10a3000d
    .word 0x00000000
    .word 0x10a00003
    .word 0x00000000
    .word 0x10000014
    .word 0x7bbf0010
    jal       func_001B0FD0
    .word 0x00000000
    .word 0x1440000f
    .word 0x00000000
    jal       func_001C6380
    .word 0x72002628
    .word 0x1000000b
    .word 0x00000000
    jal       func_001B17A0
    .word 0x00000000
    .word 0x8e02004c
    .word 0x0040f809
    .word 0x72002628
    .word 0x10000004
    .word 0x00000000
    .word 0x72002628
    jal       func_001AFC10
    .word 0x00000000
    .word 0x7bbf0010
    .word 0x7bb00000
    .word 0x03e00008
    .word 0x27bd0020
}
