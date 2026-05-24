// All-word: everything as .word except jal/j-external
extern void func_00108640(int, int, int, int);
extern void func_001086F8(int, int, int, int);

asm void func_00108FF8(void) {
    .word 0x27bdffd0
    .word 0x24050038
    .word 0xffb00000
    .word 0xffb10010
    .word 0xffbf0020
    jal       func_001086F8
    .word 0x0080802d
    .word 0x24110001
    .word 0x0200202d
    jal       func_001086F8
    .word 0x24050028
    .word 0x10000006
    .word 0x0200202d
    .word 0x00000000
    .word 0x0200202d
    jal       func_001086F8
    .word 0x24050018
    .word 0x0200202d
    jal       func_00108640
    .word 0x24050001
    .word 0x1051fff9
    .word 0xdfbf0020
    .word 0x24020001
    .word 0xdfb10010
    .word 0xdfb00000
    .word 0x03e00008
    .word 0x27bd0030
}
