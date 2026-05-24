// All-word: everything as .word except jal/j-external
extern void func_00100268(int, int, int, int);

asm void func_00100610(void) {
    .word 0x27bdffc0
    .word 0x00063400
    .word 0xffb10010
    .word 0xffb00000
    .word 0x00048c00
    .word 0xffb20020
    .word 0x00058400
    .word 0x00108403
    .word 0x00118c03
    .word 0xffbf0030
    jal       func_00100268
    .word 0x00069403
    .word 0x2603003f
    .word 0x0040282d
    .word 0x2404ffff
    .word 0x2610007e
    .word 0x0083102a
    .word 0x32310002
    .word 0x0062800b
    .word 0x12200007
    .word 0x00108183
    .word 0x2642003f
    .word 0x2643007e
    .word 0x0082202a
    .word 0x0044180b
    .word 0x10000006
    .word 0x00033183
    .word 0x2642001f
    .word 0x2643003e
    .word 0x0082202a
    .word 0x0044180b
    .word 0x00033143
    .word 0xdca20000
    .word 0x3403ffff
    .word 0x0003183c
    .word 0x3463ffff
    .word 0x24040001
    .word 0x00431024
    .word 0x14440003
    .word 0x02061018
    .word 0x10000002
    .word 0x00021400
    .word 0x00021440
    .word 0x00021403
    .word 0xdfbf0030
    .word 0xdfb20020
    .word 0xdfb10010
    .word 0xdfb00000
    .word 0x03e00008
    .word 0x27bd0040
}
