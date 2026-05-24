// CFLAGS: -O4,p -sdatathreshold 0
extern unsigned short D_70003600[];
extern unsigned short D_70003604[];
extern unsigned short D_70003608[];

asm void func_001C85D0(void *a0, void *a1) {
    .word 0x9488000 2  // lhu t0, 2(a0)
    .word 0x9486000 0  // lhu a2, 0(a0)
}
