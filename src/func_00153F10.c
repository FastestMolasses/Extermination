// All-word: everything as .word except jal/j-external (for R_MIPS_26 relocs)
extern void func_00154040(int, int, int, int);
extern void func_00154120(int, int, int, int);
extern void func_001AFC10(int, int, int, int);
extern void func_001EFE00(int, int, int, int);
extern void func_001FBD50(int, int, int, int);

asm void func_00153F10(void) {
    .word 0x27bdffe0
    .word 0x7fbf0010
    .word 0x7fb00000
    .word 0x90870004
    .word 0x70808628
    .word 0x24060003
    .word 0x10e6003c
    .word 0x260501f0
    .word 0x24030002
    .word 0x10e30019
    .word 0x00000000
    .word 0x24030001
    .word 0x10e30009
    .word 0x00000000
    .word 0x10e00003
    .word 0x00000000
    .word 0x10000035
    .word 0x7bbf0010
    jal       func_00154040
    .word 0x00000000
    .word 0x10000030
    .word 0x00000000
    .word 0x3c017000
    .word 0x90233b8d
    .word 0x10600006
    .word 0x72002628
    .word 0x306400ff
    .word 0x24030004
    .word 0x14830028
    .word 0x00000000
    .word 0x72002628
    jal       func_00154120
    .word 0x00000000
    .word 0x10000023
    .word 0x00000000
    .word 0x92050005
    .word 0x10a3001b
    .word 0x00000000
    .word 0x24030001
    .word 0x10a30016
    .word 0x24a30001
    .word 0x50a00004
    .word 0x3c024396
    .word 0x10000019
    .word 0x00000000
    .word 0x3c024396
    .word 0x44826000
    .word 0x24050434
    jal       func_001FBD50
    .word 0x70003628
    .word 0x3c028000
    .word 0x34440052
    jal       func_001EFE00
    .word 0x72002e28
    .word 0x8e02004c
    .word 0x0040f809
    .word 0x72002628
    .word 0x92030005
    .word 0x24630001
    .word 0x10000009
    .word 0xa2030005
    .word 0x24a30001
    .word 0x10000006
    .word 0xa2030005
    .word 0xa2060004
    .word 0x10000003
    .word 0xa2000005
    jal       func_001AFC10
    .word 0x00000000
    .word 0x7bbf0010
    .word 0x7bb00000
    .word 0x03e00008
    .word 0x27bd0020
}
