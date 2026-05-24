// All-word: everything as .word except jal/j-external
extern void func_001749A0(int, int, int, int);
extern void func_001B61C0(int, int, int, int);
extern void func_001FBD50(int, int, int, int);
extern void func_0021C270(int, int, int, int);
extern void func_0021C350(int, int, int, int);

asm void func_00224290(void) {
    .word 0x27bdffe0
    .word 0x7fbf0010
    .word 0x7fb00000
    .word 0x90830007
    .word 0x24020001
    .word 0x10620041
    .word 0x70808628
    .word 0x10600003
    .word 0x00000000
    .word 0x1000004a
    .word 0x24020001
    .word 0xc6020224
    .word 0x44800800
    .word 0x00000000
    .word 0x46011032
    .word 0x00000000
    .word 0x45000006
    .word 0x00000000
    .word 0xc600022c
    .word 0x46010032
    .word 0x00000000
    .word 0x4501002f
    .word 0x70001628
    .word 0x44800000
    .word 0x00000000
    .word 0x46001032
    .word 0x00000000
    .word 0x45010009
    .word 0x00000000
    .word 0x3c024396
    .word 0x44826000
    .word 0x24050152
    .word 0x72002628
    jal       func_001FBD50
    .word 0x70003628
    jal       func_0021C350
    .word 0x72002628
    .word 0xc601022c
    .word 0x44800000
    .word 0x00000000
    .word 0x46000832
    .word 0x00000000
    .word 0x45010009
    .word 0x00000000
    .word 0x3c024396
    .word 0x44826000
    .word 0x24050153
    .word 0x72002628
    jal       func_001FBD50
    .word 0x70003628
    jal       func_0021C270
    .word 0x72002628
    .word 0x92020007
    .word 0x240500c0
    .word 0x24060005
    .word 0x24070001
    .word 0x24420001
    .word 0x70002628
    jal       func_001B61C0
    .word 0xa2020007
    .word 0x3c023f80
    .word 0x44826000
    .word 0x24050076
    .word 0x72002628
    jal       func_001749A0
    .word 0x70003628
    .word 0x10000010
    .word 0x00000000
    .word 0x70001628
    .word 0x1000000f
    .word 0x7bbf0010
    .word 0x8e020200
    .word 0x30421000
    .word 0x10400009
    .word 0x00000000
    .word 0x3c023f80
    .word 0x44826000
    .word 0x24050072
    .word 0x70003628
    jal       func_001749A0
    .word 0xa2000007
    .word 0x2402003c
    .word 0xa602020e
    .word 0x24020001
    .word 0x7bbf0010
    .word 0x7bb00000
    .word 0x03e00008
    .word 0x27bd0020
}
