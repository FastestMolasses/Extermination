// All-word: everything as .word except jal/j-external
extern void func_00205F80(int, int, int, int);

asm void func_00205F90(void) {
    .word 0x27bdfff0
    .word 0x7fbf0000
    jal       func_00205F80
    .word 0x00000000
    .word 0x10400003
    .word 0x00000000
    .word 0x10000015
    .word 0x70001628
    .word 0x8c850008
    .word 0x8c83000c
    .word 0x8c860010
    .word 0x8c820004
    .word 0x00a31823
    .word 0x00c31821
    .word 0x0066001a
    .word 0x00000000
    .word 0x00000000
    .word 0x00002810
    .word 0x00051900
    .word 0x00652021
    .word 0x000418c0
    .word 0x00832021
    .word 0x00041880
    .word 0x00641823
    .word 0x000318c0
    .word 0x00651821
    .word 0x00031980
    .word 0x00431021
    .word 0x7bbf0000
    .word 0x03e00008
    .word 0x27bd0010
}
