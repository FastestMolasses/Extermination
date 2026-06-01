// All-word: everything as .word except jal/j-external (for R_MIPS_26 relocs)
extern void func_00122BB8(int, int, int, int);
extern void func_00131ED0(int, int, int, int);
extern void func_00136240(int, int, int, int);
extern void func_00136490(int, int, int, int);
extern void func_00136630(int, int, int, int);
extern void func_001366E0(int, int, int, int);
extern void func_001B17A0(int, int, int, int);
extern void func_001B5360(int, int, int, int);
extern void anim_advance_time(int, int, int, int);
extern void func_001C68C0(int, int, int, int);
extern void func_001FBD50(int, int, int, int);

asm void func_00136140(void) {
    .word 0x27bdffd0
    .word 0x7fbf0020
    .word 0x7fb10010
    .word 0x7fb00000
    .word 0x90830005
    .word 0x24020001
    .word 0x70808e28
    .word 0x10620009
    .word 0x70a08628
    .word 0x10600003
    .word 0x00000000
    .word 0x10000008
    .word 0x92030000
    jal       func_00136240
    .word 0x00000000
    .word 0x10000003
    .word 0x00000000
    jal       func_00136490
    .word 0x00000000
    .word 0x92030000
    .word 0x24020009
    .word 0x1462000e
    .word 0x72202628
    jal       func_00122BB8
    .word 0x00000000
    .word 0x00021c03
    .word 0x2402000a
    .word 0x0062001a
    .word 0x72202628
    .word 0x70003628
    .word 0x3c024396
    .word 0x44826000
    .word 0x00001010
    jal       func_001FBD50
    .word 0x24450841
    .word 0x72202628
    jal       func_001B5360
    .word 0x00000000
    .word 0x72202628
    jal       func_00136630
    .word 0x72002e28
    .word 0x72202628
    jal       func_001366E0
    .word 0x72002e28
    .word 0x3c023f80
    .word 0x44826000
    jal       anim_advance_time
    .word 0x72202628
    .word 0xae020000
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
