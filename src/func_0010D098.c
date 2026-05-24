// All-word: everything as .word except jal/j-external
extern void func_00125848(int, int, int, int);
extern void func_00126EC8(int, int, int, int);

asm void func_0010D098(void) {
    .word 0x27bdfff0
    .word 0x0000282d
    .word 0xffbf0000
    .word 0x14800003
    .word 0x0000302d
    .word 0x10000029
    .word 0x0000102d
    .word 0x04810004
    .word 0x0080182d
    .word 0x0004182f
    .word 0x34068000
    .word 0x0006343c
    .word 0x24020001
    .word 0x5062000a
    .word 0x28a20035
    .word 0x00000000
    .word 0x64a50001
    .word 0x0003187b
    .word 0x00000000
    .word 0x00000000
    .word 0x00000000
    .word 0x1462fffa
    .word 0x00000000
    .word 0x28a20035
    .word 0x10400007
    .word 0x64a303ff
    .word 0x24020040
    .word 0x0045102f
    .word 0x0002103c
    .word 0x0002103f
    .word 0x00442014
    .word 0x0004233b
    .word 0x0003153c
    .word 0x00c21025
    .word 0x00442025
    .word 0x04800005
    .word 0x0004107a
    jal       func_00125848
    .word 0x00000000
    .word 0x10000008
    .word 0xdfbf0000
    .word 0x30840001
    jal       func_00125848
    .word 0x00822025
    .word 0x0040202d
    jal       func_00126EC8
    .word 0x0080282d
    .word 0xdfbf0000
    .word 0x03e00008
    .word 0x27bd0010
}
