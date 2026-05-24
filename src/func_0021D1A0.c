// All-word: everything as .word except jal/j-external
extern void func_0011DF78(int, int, int, int);
extern void func_0011E620(int, int, int, int);
extern void func_001B1470(int, int, int, int);

asm void func_0021D1A0(void) {
    .word 0x27bdffe0
    .word 0x7fbf0010
    .word 0x7fb00000
    .word 0xc4800078
    .word 0xc48d0070
    .word 0x70808628
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
    .word 0x3c017000
    .word 0xe4203a20
    .word 0x3c017000
    .word 0xc60100c4
    .word 0xc4203a20
    jal       func_001B1470
    .word 0x46010301
    .word 0x3c017000
    .word 0xe4203a24
    jal       func_0011DF78
    .word 0x46000306
    .word 0x3c023fc9
    .word 0x34420fdb
    .word 0x44820800
    .word 0x00000000
    .word 0x46010036
    .word 0x00000000
    .word 0x45000004
    .word 0x24020001
    .word 0x10000002
    .word 0x70001628
    .word 0x24020001
    .word 0x7bbf0010
    .word 0x7bb00000
    .word 0x03e00008
    .word 0x27bd0020
}
