// All-word: everything as .word except jal/j-external
extern void func_00163C10(int, int, int, int);
extern void func_00163D50(int, int, int, int);
extern void func_00163E90(int, int, int, int);
extern void func_00164220(int, int, int, int);
extern void func_001643B0(int, int, int, int);
extern void func_0021D2E0(int, int, int, int);

asm void func_00163B40(void) {
    .word 0x27bdfff0
    .word 0x7fbf0000
    .word 0x90850006
    .word 0x2403000a
    .word 0x10a30029
    .word 0x00000000
    .word 0x24030005
    .word 0x10a30022
    .word 0x00000000
    .word 0x24030004
    .word 0x10a3001b
    .word 0x00000000
    .word 0x24030003
    .word 0x10a30014
    .word 0x00000000
    .word 0x24030002
    .word 0x10a3000d
    .word 0x00000000
    .word 0x24030001
    .word 0x50a30006
    .word 0x24050078
    .word 0x10a0001a
    .word 0x00000000
    .word 0x10000019
    .word 0x7bbf0000
    .word 0x24050078
    jal       func_0021D2E0
    .word 0x70003628
    .word 0x10000013
    .word 0x00000000
    jal       func_00164220
    .word 0x00000000
    .word 0x1000000f
    .word 0x00000000
    jal       func_00163E90
    .word 0x00000000
    .word 0x1000000b
    .word 0x00000000
    jal       func_00163D50
    .word 0x00000000
    .word 0x10000007
    .word 0x00000000
    jal       func_00163C10
    .word 0x00000000
    .word 0x10000003
    .word 0x00000000
    jal       func_001643B0
    .word 0x00000000
    .word 0x7bbf0000
    .word 0x03e00008
    .word 0x27bd0010
}
