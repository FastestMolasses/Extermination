// All-word: everything as .word except jal/j-external
extern void func_001749A0(int, int, int, int);
extern void func_00175900(int, int, int, int);
extern void func_001764E0(int, int, int, int);
extern void func_00179880(int, int, int, int);
extern void func_0017C580(int, int, int, int);
extern void func_0017C860(int, int, int, int);
extern void func_0021D250(int, int, int, int);
extern void func_0021D2E0(int, int, int, int);
extern void func_00224290(int, int, int, int);

asm void func_001639E0(void) {
    .word 0x27bdffc0
    .word 0x7fbf0030
    .word 0x7fb20020
    .word 0x7fb10010
    .word 0x7fb00000
    .word 0x90850006
    .word 0x24030002
    .word 0x70808e28
    .word 0x10a30046
    .word 0x70008628
    .word 0x24030001
    .word 0x50a30010
    .word 0x72202628
    .word 0x50a00004
    .word 0x24a20001
    .word 0x10000043
    .word 0x7bbf0030
    .word 0x24a20001
    .word 0xa2220006
    .word 0x3c024100
    .word 0xa2200007
    .word 0xae200038
    .word 0x44826000
    .word 0x24050072
    .word 0x70003628
    jal       func_001749A0
    .word 0xa220025c
    .word 0x72202628
    jal       func_00224290
    .word 0x00000000
    .word 0x9223023b
    .word 0x70409628
    .word 0x24020039
    .word 0x1462000b
    .word 0x00000000
    .word 0xc62102f4
    .word 0x3c024140
    .word 0x44820000
    .word 0xc62200b4
    .word 0x46000801
    .word 0x46001034
    .word 0x00000000
    .word 0x45010002
    .word 0x00000000
    .word 0x24100001
    .word 0x16400009
    .word 0x72202628
    .word 0x16000006
    .word 0x00000000
    .word 0xc62c02ec
    jal       func_0017C860
    .word 0x72202628
    .word 0x1440001d
    .word 0x00000000
    .word 0x72202628
    jal       func_001764E0
    .word 0x00000000
    .word 0x262502ec
    jal       func_00179880
    .word 0x72202628
    .word 0x24050001
    jal       func_00175900
    .word 0x72202628
    .word 0x9223000a
    .word 0x10600005
    .word 0x00000000
    .word 0x16400003
    .word 0x00000000
    jal       func_0017C580
    .word 0x72202628
    .word 0x9224023a
    .word 0x2403005d
    .word 0x14830009
    .word 0x00000000
    .word 0x72202628
    jal       func_0021D250
    .word 0x70002e28
    .word 0x10000004
    .word 0x00000000
    .word 0x24050078
    jal       func_0021D2E0
    .word 0x70003628
    .word 0x7bbf0030
    .word 0x7bb20020
    .word 0x7bb10010
    .word 0x7bb00000
    .word 0x03e00008
    .word 0x27bd0040
}
