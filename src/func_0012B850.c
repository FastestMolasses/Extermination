// All-word: everything as .word except jal/j-external
extern void func_00102948(int, int, int, int);
extern void func_00128640(int, int, int, int);
extern void func_001287F0(int, int, int, int);
extern void func_001C2770(int, int, int, int);
extern void func_001C3D60(int, int, int, int);

asm void func_0012B850(void) {
    .word 0x27bdffc0
    .word 0x7fbf0030
    .word 0x7fb20020
    .word 0x7fb10010
    .word 0x70809628
    .word 0x70a08e28
    .word 0x70003628
    jal       func_001C2770
    .word 0x7fb00000
    .word 0x92440006
    .word 0x24030002
    .word 0x10830024
    .word 0x70408628
    .word 0x24030001
    .word 0x10830015
    .word 0x00000000
    .word 0x50800004
    .word 0x24020078
    .word 0x10000028
    .word 0x00000000
    .word 0x24020078
    .word 0xa62200d0
    .word 0x92450006
    .word 0x3c023e99
    .word 0x3443999a
    .word 0x3c024100
    .word 0x44826000
    .word 0x24060006
    .word 0x24a20001
    .word 0xa2420006
    .word 0xae2300d8
    .word 0xc64000c4
    .word 0x72402628
    .word 0x72202e28
    jal       func_001287F0
    .word 0xe62000e8
    .word 0x862300d0
    .word 0x2463ffff
    .word 0xa62300d0
    .word 0x00031c3c
    .word 0x00031c3f
    .word 0x14600011
    .word 0x00000000
    .word 0x92430006
    .word 0x24630001
    .word 0xa2430006
    .word 0x1000000c
    .word 0xae2000d8
    jal       func_00128640
    .word 0x72402628
    .word 0x14400008
    .word 0x00000000
    .word 0x26240050
    jal       func_00102948
    .word 0x264500b0
    .word 0x24030001
    .word 0xa2430005
    .word 0xa2400006
    .word 0xa2400007
    .word 0x16000004
    .word 0x00000000
    .word 0x72402628
    jal       func_001C3D60
    .word 0x72202e28
    .word 0x7bbf0030
    .word 0x7bb20020
    .word 0x7bb10010
    .word 0x7bb00000
    .word 0x03e00008
    .word 0x27bd0040
}
