// All-word: everything as .word except jal/j-external
extern void func_00132490(int, int, int, int);
extern void anim_clip_init(int, int, int, int);
extern void func_001FBD50(int, int, int, int);

asm void func_0012F980(void) {
    .word 0x27bdffd0
    .word 0x7fbf0020
    .word 0x7fb10010
    .word 0x7fb00000
    .word 0x90830006
    .word 0x24020001
    .word 0x70a08628
    .word 0x1062000f
    .word 0x70808e28
    .word 0x10600004
    .word 0x24620001
    .word 0x1000001f
    .word 0x72202628
    .word 0x24620001
    .word 0x44806800
    .word 0xa2220006
    .word 0x3c023f80
    .word 0xae020034
    .word 0x3c0240a0
    .word 0x44826000
    .word 0x2405000b
    jal       anim_clip_init
    .word 0xae00003c
    .word 0x96020058
    .word 0x30421000
    .word 0x10400010
    .word 0x00000000
    .word 0x24020004
    .word 0xa2220005
    .word 0x3c023ecc
    .word 0xa2200006
    .word 0x3442cccd
    .word 0xae02003c
    .word 0x3c024006
    .word 0x34426666
    .word 0xae020038
    .word 0x3c024396
    .word 0x44826000
    .word 0x240507d3
    .word 0x72202628
    jal       func_001FBD50
    .word 0x70003628
    .word 0x72202628
    jal       func_00132490
    .word 0x72002e28
    .word 0x7bbf0020
    .word 0x7bb10010
    .word 0x7bb00000
    .word 0x03e00008
    .word 0x27bd0030
}
