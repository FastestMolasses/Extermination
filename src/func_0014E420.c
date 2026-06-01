// All-word: everything as .word except jal/j-external
extern void anim_clip_init(int, int, int, int);
extern void func_001FBD50(int, int, int, int);

asm void func_0014E420(void) {
    .word 0x27bdffe0
    .word 0x7fbf0010
    .word 0x7fb00000
    .word 0x90830006
    .word 0x24060001
    .word 0x1066001e
    .word 0x70808628
    .word 0x50600004
    .word 0x24620001
    .word 0x10000024
    .word 0x7bbf0010
    .word 0x24620001
    .word 0x44806800
    .word 0xa2020006
    .word 0xaca00028
    .word 0x3c023f80
    .word 0xaca20020
    .word 0x240200f0
    .word 0xa2060007
    .word 0xa4a20056
    .word 0x3c024020
    .word 0xaca2003c
    .word 0xa0a00059
    .word 0xa0a00058
    .word 0x3c0240a0
    .word 0x44826000
    jal       anim_clip_init
    .word 0x24050017
    .word 0x3c024396
    .word 0x44826000
    .word 0x24050867
    .word 0x72002628
    jal       func_001FBD50
    .word 0x70003628
    .word 0x1000000a
    .word 0x00000000
    .word 0x240300f0
    .word 0xa4a30056
    .word 0x8ca30000
    .word 0x30631000
    .word 0x10600004
    .word 0x00000000
    .word 0xa2060004
    .word 0xa2000005
    .word 0xa2000006
    .word 0x7bbf0010
    .word 0x7bb00000
    .word 0x03e00008
    .word 0x27bd0020
}
