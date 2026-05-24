// All-word: everything as .word except jal/j-external
extern void func_001AEDE0(int, int, int, int);

asm void func_0015D460(void) {
    .word 0x27bdfff0
    .word 0x7fbf0000
    .word 0x90850005
    .word 0x24030001
    .word 0x50a3000b
    .word 0x24a20001
    .word 0x10a00004
    .word 0x24a30001
    .word 0x1000000c
    .word 0x7bbf0000
    .word 0x24a30001
    .word 0xa0830005
    .word 0xac800220
    .word 0x10000006
    .word 0xa0800000
    .word 0x24a20001
    .word 0xa0820005
    .word 0x24040004
    jal       func_001AEDE0
    .word 0x70002e28
    .word 0x7bbf0000
    .word 0x03e00008
    .word 0x27bd0010
}
