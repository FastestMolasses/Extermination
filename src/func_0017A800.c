// All-word: everything as .word except jal/j-external
extern void func_0011E620(int, int, int, int);
extern void func_0011E748(int, int, int, int);
extern void func_001B1510(int, int, int, int);

asm void func_0017A800(void) {
    .word 0x27bdffd0
    .word 0x7fbf0020
    .word 0x7fb10010
    .word 0x7fb00000
    .word 0xc4a10000
    .word 0xc4800000
    .word 0x3c017000
    .word 0x70808e28
    .word 0x70a08628
    .word 0x46000801
    .word 0xe4203a20
    .word 0xc4a10008
    .word 0xc4800008
    .word 0x3c017000
    .word 0xc4223a20
    .word 0x46000801
    .word 0x3c017000
    .word 0xe4203a28
    .word 0x4602101a
    .word 0x46000006
    jal       func_0011E748
    .word 0x4600031c
    .word 0x3c017000
    .word 0xe4203a2c
    .word 0xc6010004
    .word 0xc6200004
    .word 0x3c017000
    .word 0xc42c3a2c
    .word 0x46000801
    .word 0x3c017000
    .word 0xe4203a24
    jal       func_0011E620
    .word 0x46000346
    .word 0x3c017000
    .word 0xe4203a20
    jal       func_001B1510
    .word 0x46000306
    .word 0x7bbf0020
    .word 0x7bb10010
    .word 0x7bb00000
    .word 0x03e00008
    .word 0x27bd0030
}
