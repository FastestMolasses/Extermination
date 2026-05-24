// All-word: everything as .word except jal/j-external
extern void func_001749A0(int, int, int, int);
extern void func_00174AC0(int, int, int, int);
extern void func_00175900(int, int, int, int);
extern void func_001764E0(int, int, int, int);
extern void func_00178B90(int, int, int, int);
extern void func_001796C0(int, int, int, int);
extern void func_0017C440(int, int, int, int);
extern void func_0017C540(int, int, int, int);

asm void func_00163C10(void) {
    .word 0x27bdffe0
    .word 0x7fbf0010
    .word 0x7fb00000
    .word 0x90830007
    .word 0x24020003
    .word 0x1062002f
    .word 0x70808628
    .word 0x24020002
    .word 0x10620019
    .word 0x70002e28
    .word 0x24020001
    .word 0x1062000e
    .word 0x00000000
    .word 0x10600004
    .word 0x24620001
    .word 0x1000002e
    .word 0x72002628
    .word 0x24620001
    .word 0xa2020007
    .word 0x3c023f80
    .word 0x44826000
    .word 0x2405006e
    jal       func_001749A0
    .word 0x70003628
    .word 0x10000024
    .word 0x00000000
    .word 0x8e020200
    .word 0x30421000
    .word 0x10400020
    .word 0x00000000
    .word 0x24620001
    .word 0x1000001d
    .word 0xa2020007
    .word 0x70002e28
    jal       func_00174AC0
    .word 0x00000000
    .word 0x9202023f
    .word 0x28410002
    .word 0x1420000a
    .word 0x72002628
    .word 0x92020007
    .word 0x72002628
    .word 0x70002e28
    .word 0x24420001
    jal       func_0017C440
    .word 0xa2020007
    .word 0x1000000e
    .word 0x00000000
    .word 0x72002628
    jal       func_0017C540
    .word 0xa200025c
    .word 0x10000009
    .word 0x00000000
    jal       func_00178B90
    .word 0x70002e28
    .word 0x8e020200
    .word 0x30428000
    .word 0x14400003
    .word 0x00000000
    jal       func_0017C540
    .word 0x72002628
    .word 0x72002628
    jal       func_001764E0
    .word 0x00000000
    .word 0xc60100b4
    .word 0x3c02be4c
    .word 0x3442cccd
    .word 0x44820000
    .word 0x24050001
    .word 0x46000800
    .word 0x72002628
    jal       func_00175900
    .word 0xe60000b4
    jal       func_001796C0
    .word 0x72002628
    .word 0x7bbf0010
    .word 0x7bb00000
    .word 0x03e00008
    .word 0x27bd0020
}
