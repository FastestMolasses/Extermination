// All-word: everything as .word except jal/j-external
extern void CreateSema(int, int, int, int);
extern void SignalSema(int, int, int, int);

asm void func_00204390(void) {
    .word 0x27bdffa0
    .word 0x7fbf0050
    .word 0x7fb40040
    .word 0x7fb30030
    .word 0x7fb20020
    .word 0x7fb10010
    .word 0x7fb00000
    .word 0x7080a628
    .word 0x8c840040
    .word 0x70a09e28
    .word 0x70c09628
    .word 0x70e08e28
    jal       SignalSema
    .word 0x71008628
    .word 0x8e850010
    .word 0x8e83000c
    .word 0x8e820008
    .word 0x8e840014
    .word 0x8e860018
    .word 0x00651821
    .word 0x00031ac0
    .word 0x00641821
    .word 0x0066001a
    .word 0x2442fffe
    .word 0x00451023
    .word 0x000212c0
    .word 0x00442023
    .word 0x00001810
    .word 0x00c31023
    .word 0x0044102a
    .word 0x14400008
    .word 0x00000000
    .word 0x8e820000
    .word 0x00431021
    .word 0xae620000
    .word 0xae440000
    .word 0xae200000
    .word 0x1000000d
    .word 0xae000000
    .word 0x8e820000
    .word 0x00431021
    .word 0xae620000
    .word 0x8e820018
    .word 0x00431023
    .word 0xae420000
    .word 0x8e820000
    .word 0xae220000
    .word 0x8e820018
    .word 0x00431023
    .word 0x00821023
    .word 0xae020000
    jal       CreateSema
    .word 0x8e840040
    .word 0x7bbf0050
    .word 0x7bb40040
    .word 0x7bb30030
    .word 0x7bb20020
    .word 0x7bb10010
    .word 0x7bb00000
    .word 0x03e00008
    .word 0x27bd0060
}
