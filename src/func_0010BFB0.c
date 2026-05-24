// All-word: everything as .word except jal/j-external
extern void iGsGetIMR(int, int, int, int);

asm void func_0010BFB0(void) {
    .word 0x27bdffe0
    .word 0x24060007
    .word 0xffbf0010
    .word 0x03a0282d
    .word 0x00061080
    .word 0x00441006
    .word 0x3042000f
    .word 0x2843000a
    .word 0x50600002
    .word 0x24420057
    .word 0x24420030
    .word 0xa0a20000
    .word 0x24c6ffff
    .word 0x04c1fff6
    .word 0x24a50001
    .word 0xa0a00000
    jal       iGsGetIMR
    .word 0x03a0202d
    .word 0xdfbf0010
    .word 0x03e00008
    .word 0x27bd0020
}
