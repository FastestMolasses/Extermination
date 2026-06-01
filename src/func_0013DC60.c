// All-word: everything as .word except jal/j-external
extern void anim_clip_init(int, int, int, int);
extern void func_001FBD50(int, int, int, int);

asm void func_0013DC60(void) {
    .word 0x27bdffd0
    .word 0x7fbf0020
    .word 0x7fb10010
    .word 0x7fb00000
    .word 0x70a08628
    .word 0x90850006
    .word 0x24030001
    .word 0x10a30013
    .word 0x70808e28
    .word 0x50a00004
    .word 0x24a20001
    .word 0x10000028
    .word 0x7bbf0020
    .word 0x24a20001
    .word 0x44806800
    .word 0xa2220006
    .word 0x3c023f80
    .word 0xae020040
    .word 0x3c024120
    .word 0xae000048
    .word 0xae000050
    .word 0x44826000
    .word 0x2405000a
    jal       anim_clip_init
    .word 0xae00004c
    .word 0x10000019
    .word 0x00000000
    .word 0x8623002c
    .word 0x30638000
    .word 0x1460000e
    .word 0x00000000
    .word 0xc621003c
    .word 0x3c034278
    .word 0x44830000
    .word 0x00000000
    .word 0x46010032
    .word 0x00000000
    .word 0x45000006
    .word 0x00000000
    .word 0x3c024396
    .word 0x44826000
    .word 0x24050806
    jal       func_001FBD50
    .word 0x70003628
    .word 0x8e030030
    .word 0x30631000
    .word 0x10600004
    .word 0x00000000
    .word 0x24030004
    .word 0xa2230005
    .word 0xa2200006
    .word 0x7bbf0020
    .word 0x7bb10010
    .word 0x7bb00000
    .word 0x03e00008
    .word 0x27bd0030
}
