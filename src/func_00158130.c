// All-word: everything as .word except jal/j-external
extern void func_001FB9F0(int, int, int, int);

asm void func_00158130(void) {
    .word 0x27bdfff0
    .word 0x7fbf0000
    .word 0x90830003
    .word 0x24020012
    .word 0x10620005
    .word 0x24051000
    .word 0x2402002f
    .word 0x14620009
    .word 0x24051000
    .word 0x24051000
    .word 0x240403f3
    .word 0x70a03628
    jal       func_001FB9F0
    .word 0x70a03e28
    .word 0x10000007
    .word 0x7bbf0000
    .word 0x24051000
    .word 0x240403f4
    .word 0x70a03628
    jal       func_001FB9F0
    .word 0x70a03e28
    .word 0x7bbf0000
    .word 0x24020001
    .word 0x03e00008
    .word 0x27bd0010
}
