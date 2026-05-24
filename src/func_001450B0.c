// All-word: everything as .word except jal/j-external (for R_MIPS_26 relocs)
extern void func_00122BB8(int, int, int, int);
extern void func_00131ED0(int, int, int, int);
extern void func_00145210(int, int, int, int);
extern void func_001453C0(int, int, int, int);
extern void func_001455A0(int, int, int, int);
extern void func_00145880(int, int, int, int);
extern void func_001459A0(int, int, int, int);
extern void func_00146CE0(int, int, int, int);
extern void func_001B17A0(int, int, int, int);
extern void func_001C64F0(int, int, int, int);
extern void func_001C68C0(int, int, int, int);
extern void func_001FBD50(int, int, int, int);

asm void func_001450B0(void) {
    .word 0x27bdffd0
    .word 0x7fbf0020
    .word 0x7fb10010
    .word 0x7fb00000
    .word 0x90830005
    .word 0x24020002
    .word 0x70808e28
    .word 0x10620010
    .word 0x70a08628
    .word 0x24020001
    .word 0x10620009
    .word 0x00000000
    .word 0x10600003
    .word 0x00000000
    .word 0x1000000c
    .word 0x92030030
    jal       func_00145210
    .word 0x00000000
    .word 0x10000007
    .word 0x00000000
    jal       func_001453C0
    .word 0x00000000
    .word 0x10000003
    .word 0x00000000
    jal       func_001455A0
    .word 0x00000000
    .word 0x92030030
    .word 0x24020009
    .word 0x14620010
    .word 0x24020008
    jal       func_00122BB8
    .word 0x00000000
    .word 0x00021b43
    .word 0x24020005
    .word 0x0062001a
    .word 0x72202628
    .word 0x70003628
    .word 0x3c024396
    .word 0x44826000
    .word 0x00001010
    jal       func_001FBD50
    .word 0x24450827
    .word 0x10000011
    .word 0x72202628
    .word 0x24020008
    .word 0x1462000d
    .word 0x00000000
    jal       func_00122BB8
    .word 0x00000000
    .word 0x00021b43
    .word 0x24020005
    .word 0x0062001a
    .word 0x72202628
    .word 0x70003628
    .word 0x3c024396
    .word 0x44826000
    .word 0x00001010
    jal       func_001FBD50
    .word 0x2445082c
    .word 0x72202628
    jal       func_00146CE0
    .word 0x72002e28
    .word 0x72202628
    jal       func_00145880
    .word 0x72002e28
    .word 0x72202628
    jal       func_001459A0
    .word 0x72002e28
    .word 0x3c023f80
    .word 0x44826000
    jal       func_001C64F0
    .word 0x72202628
    .word 0xae020030
    jal       func_00131ED0
    .word 0x72202628
    jal       func_001C68C0
    .word 0x72202628
    jal       func_001B17A0
    .word 0x72202628
    .word 0x8e22004c
    .word 0x0040f809
    .word 0x72202628
    .word 0x7bbf0020
    .word 0x7bb10010
    .word 0x7bb00000
    .word 0x03e00008
    .word 0x27bd0030
}
