// All-word: everything as .word except jal/j-external
extern void func_0011E2A8(int, int, int, int);
extern void func_00175900(int, int, int, int);
extern void func_00178B90(int, int, int, int);
extern void func_001796C0(int, int, int, int);
extern void func_0017B5C0(int, int, int, int);
extern void func_0017BC40(int, int, int, int);
extern void func_0017C030(int, int, int, int);

asm void func_001837B0(void) {
    .word 0x27bdffe0
    .word 0x7fbf0010
    .word 0x7fb00000
    .word 0x90830006
    .word 0x24020002
    .word 0x10620018
    .word 0x70808628
    .word 0x24020001
    .word 0x10620009
    .word 0x00000000
    .word 0x10600004
    .word 0x24620001
    .word 0x1000001e
    .word 0xc60c009c
    .word 0x24620001
    .word 0xa2020006
    jal       func_0017B5C0
    .word 0xa2000007
    .word 0x8e020200
    .word 0x30428000
    .word 0x14400015
    .word 0x00000000
    .word 0x92030006
    .word 0x24020001
    .word 0x24630001
    .word 0xa2030006
    .word 0xa200025c
    .word 0xa20201f0
    .word 0x1000000d
    .word 0xa20201f1
    jal       func_0017BC40
    .word 0x00000000
    jal       func_0017C030
    .word 0x72002628
    .word 0x24050001
    jal       func_00178B90
    .word 0x72002628
    .word 0x920201f0
    .word 0x14400003
    .word 0x00000000
    .word 0xa2000005
    .word 0xa2000006
    .word 0xc60c009c
    jal       func_0011E2A8
    .word 0x00000000
    .word 0xc6030038
    .word 0x3c02be4c
    .word 0x3442cccd
    .word 0xc60200b4
    .word 0x44820800
    .word 0x24050001
    .word 0x46001802
    .word 0x72002628
    .word 0x46001001
    .word 0xe60000b4
    .word 0x46010000
    jal       func_00175900
    .word 0xe60000b4
    jal       func_001796C0
    .word 0x72002628
    .word 0x7bbf0010
    .word 0x7bb00000
    .word 0x03e00008
    .word 0x27bd0020
}
