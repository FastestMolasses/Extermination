// All-word: everything as .word except jal/j-external
extern void func_0011E620(int, int, int, int);
extern void func_001B1470(int, int, int, int);
extern void func_0021C270(int, int, int, int);
extern void func_0021C350(int, int, int, int);

asm void func_0021D6C0(void) {
    .word 0x27bdffe0
    .word 0x44800800
    .word 0x7fbf0010
    .word 0x7fb00000
    .word 0xc4800224
    .word 0x46010032
    .word 0x00000000
    .word 0x4500000a
    .word 0x70808628
    .word 0xc600022c
    .word 0x46010032
    .word 0x00000000
    .word 0x45000005
    .word 0x00000000
    .word 0x9202000f
    .word 0x30420002
    .word 0x10400038
    .word 0x70001628
    .word 0xc601022c
    .word 0x44800000
    .word 0x00000000
    .word 0x46000832
    .word 0x00000000
    .word 0x45010005
    .word 0x00000000
    jal       func_0021C270
    .word 0x72002628
    .word 0x24020001
    .word 0xa20201f1
    .word 0xc6010224
    .word 0x44800000
    .word 0x00000000
    .word 0x46000832
    .word 0x00000000
    .word 0x45010004
    .word 0x00000000
    jal       func_0021C350
    .word 0x72002628
    .word 0xa20001f1
    .word 0x8e020020
    .word 0xc44000c8
    .word 0xc44d00c0
    jal       func_0011E620
    .word 0x46000307
    .word 0x3c017000
    .word 0xe4203a20
    .word 0x3c017000
    .word 0xc4203a20
    .word 0x3c023fc9
    .word 0x34420fdb
    .word 0x44820800
    jal       func_001B1470
    .word 0x46000b00
    .word 0xe60000c4
    .word 0xae0000c0
    .word 0x920301f0
    .word 0x24020033
    .word 0x14620006
    .word 0x24020002
    .word 0xc60002e0
    .word 0xe600027c
    .word 0xc60002e4
    .word 0xe6000278
    .word 0x24020002
    .word 0xa2020004
    .word 0x2402000a
    .word 0xa2020005
    .word 0x24020046
    .word 0xa2000006
    .word 0xa20201f0
    .word 0x10000002
    .word 0x24020001
    .word 0x70001628
    .word 0x7bbf0010
    .word 0x7bb00000
    .word 0x03e00008
    .word 0x27bd0020
}
