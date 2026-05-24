// All-word: everything as .word except jal/j-external
extern void func_00102948(int, int, int, int);
extern void func_00122BB8(int, int, int, int);
extern void func_001287F0(int, int, int, int);
extern void func_001C2770(int, int, int, int);
extern void func_001C3D60(int, int, int, int);

asm void func_0012D850(void) {
    .word 0x27bdffc0
    .word 0x7fbf0030
    .word 0x7fb20020
    .word 0x7fb10010
    .word 0x24060002
    .word 0x70809628
    .word 0x70a08e28
    jal       func_001C2770
    .word 0x7fb00000
    .word 0x92440006
    .word 0x24030001
    .word 0x10830015
    .word 0x70408628
    .word 0x50800004
    .word 0x24820001
    .word 0x1000001f
    .word 0x00000000
    .word 0x24820001
    jal       func_00122BB8
    .word 0xa2420006
    .word 0x30420030
    .word 0x2442003c
    .word 0x44806000
    .word 0xa62200d0
    .word 0x3c023f19
    .word 0x3442999a
    .word 0x24060006
    .word 0xae2200d8
    .word 0x72402628
    jal       func_001287F0
    .word 0x72202e28
    .word 0x1000000f
    .word 0x00000000
    .word 0x862300d0
    .word 0x2463ffff
    .word 0xa62300d0
    .word 0x00031c3c
    .word 0x00031c3f
    .word 0x14600008
    .word 0x00000000
    .word 0x26240050
    jal       func_00102948
    .word 0x264500b0
    .word 0x3c033f80
    .word 0xae2300ec
    .word 0xae2000d8
    .word 0xa2400005
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
