// All-word: everything as .word except jal/j-external
extern void func_00119890(int, int, int, int);
extern void func_0011A070(int, int, int, int);
extern void func_0011A218(int, int, int, int);
extern void func_001FBF50(int, int, int, int);

asm void func_001FBDB0(void) {
    .word 0x27bdffb0
    .word 0x7fbf0030
    .word 0x7fb10020
    .word 0x7fb00010
    .word 0xe7b50004
    .word 0xe7b40000
    .word 0x2402ffff
    .word 0x46006546
    .word 0x70a08628
    .word 0x70808e28
    .word 0x16020003
    .word 0x46006d06
    .word 0x1000001e
    .word 0x7bbf0030
    jal       func_00119890
    .word 0x24040001
    .word 0x24030002
    .word 0x10430004
    .word 0x27a50048
    .word 0x10000016
    .word 0x2402ffff
    .word 0x27a50048
    .word 0x4600ab06
    .word 0x27a6004c
    .word 0x4600a346
    .word 0x72202628
    jal       func_001FBF50
    .word 0x70003e28
    .word 0x10400008
    .word 0x72002628
    .word 0x8fa50048
    .word 0x8fa6004c
    jal       func_0011A218
    .word 0x72002628
    .word 0x10000007
    .word 0x72001628
    .word 0x72002628
    jal       func_0011A070
    .word 0x00000000
    .word 0x10000002
    .word 0x2402ffff
    .word 0x72001628
    .word 0x7bbf0030
    .word 0x7bb10020
    .word 0x7bb00010
    .word 0xc7b50004
    .word 0xc7b40000
    .word 0x03e00008
    .word 0x27bd0050
}
