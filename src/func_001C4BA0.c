// All-word: everything as .word except jal/j-external (for R_MIPS_26 relocs)
extern void copy_qw4(int, int, int, int);
extern void func_001031E0(int, int, int, int);
extern void func_001AFC10(int, int, int, int);
extern void func_001B0FD0(int, int, int, int);
extern void func_001B17A0(int, int, int, int);
extern void func_001C6380(int, int, int, int);
extern void func_001CA5E0(int, int, int, int);

asm void func_001C4BA0(void) {
    .word 0x27bdffe0
    .word 0x7fbf0010
    .word 0x7fb00000
    .word 0x90850004
    .word 0x8c860018
    .word 0x24030003
    .word 0x10a30034
    .word 0x70808628
    .word 0x24030002
    .word 0x50a30032
    .word 0x72002628
    .word 0x24030001
    .word 0x10a30011
    .word 0x00000000
    .word 0x10a00003
    .word 0x00000000
    .word 0x1000002e
    .word 0x7bbf0010
    jal       func_001B0FD0
    .word 0x00000000
    .word 0x14400029
    .word 0x00000000
    .word 0x8e050044
    .word 0x24060001
    jal       func_001CA5E0
    .word 0x72002628
    jal       func_001C6380
    .word 0x72002628
    .word 0x10000021
    .word 0x00000000
    .word 0x92030003
    .word 0x24020004
    .word 0x1462000d
    .word 0x00000000
    .word 0x8cc20018
    .word 0x8e030110
    .word 0x8c420110
    .word 0x24640090
    jal       copy_qw4
    .word 0x24450090
    .word 0x8e020110
    .word 0x260500b0
    jal       func_001031E0
    .word 0x244400c0
    .word 0x10000007
    .word 0x72002628
    .word 0x8cc30110
    .word 0x8e020110
    .word 0xc460007c
    jal       func_001C6380
    .word 0xe440007c
    .word 0x72002628
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
