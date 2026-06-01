// All-word: everything as .word except jal/j-external (for R_MIPS_26 relocs)
extern void func_001AFC10(int, int, int, int);
extern void func_001B0FD0(int, int, int, int);
extern void func_001B1D20(int, int, int, int);
extern void func_001BBD20(int, int, int, int);
extern void bone_wobble_decay_1(int, int, int, int);
extern void bone_wobble_decay_0(int, int, int, int);
extern void func_001C6380(int, int, int, int);

asm void func_001BDE60(void) {
    .word 0x27bdffd0
    .word 0x7fbf0020
    .word 0x7fb10010
    .word 0x7fb00000
    .word 0x90870004
    .word 0x70808e28
    .word 0x24030003
    .word 0x10e30046
    .word 0x263001f0
    .word 0x24060002
    .word 0x50e60044
    .word 0x72202628
    .word 0x24050001
    .word 0x10e5000e
    .word 0x00000000
    .word 0x10e00003
    .word 0x00000000
    .word 0x10000040
    .word 0x7bbf0020
    jal       func_001B0FD0
    .word 0x00000000
    .word 0x24030001
    .word 0xa2230000
    .word 0x9223002e
    .word 0xa6230034
    .word 0xa620002e
    .word 0x10000036
    .word 0xae000010
    .word 0x92230005
    .word 0x5066001f
    .word 0x72002628
    .word 0x5065000f
    .word 0x72002628
    .word 0x10600003
    .word 0x00000000
    .word 0x1000001f
    .word 0x72202628
    .word 0x9222000b
    .word 0x1040001b
    .word 0x00000000
    .word 0x24620001
    .word 0xa2220005
    jal       func_001BBD20
    .word 0x70002e28
    .word 0x10000015
    .word 0x00000000
    .word 0x72002628
    jal       bone_wobble_decay_1
    .word 0x00000000
    .word 0x10400010
    .word 0x00000000
    .word 0x92220005
    .word 0x24050001
    .word 0x72202628
    .word 0x24420001
    .word 0xa2220005
    jal       func_001BBD20
    .word 0xa220000b
    .word 0x10000007
    .word 0x00000000
    .word 0x72002628
    jal       bone_wobble_decay_0
    .word 0x00000000
    .word 0x10400002
    .word 0x00000000
    .word 0xa2200005
    .word 0x72202628
    jal       func_001C6380
    .word 0x00000000
    .word 0x24020001
    .word 0xa2220001
    jal       func_001B1D20
    .word 0x72202628
    .word 0x8e22004c
    .word 0x0040f809
    .word 0x72202628
    .word 0x10000004
    .word 0x00000000
    .word 0x72202628
    jal       func_001AFC10
    .word 0x00000000
    .word 0x7bbf0020
    .word 0x7bb10010
    .word 0x7bb00000
    .word 0x03e00008
    .word 0x27bd0030
}
