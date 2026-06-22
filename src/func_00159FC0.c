// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// If bit 0x4 of the flags byte at +0xB is set: program the absolute-address
// parameter block at 0x700038A0 (0.3, 0.0, 5.0, 1.0), kick off three
// subsystem calls (func_001B6F00 seeded with pi and &the block, then
// func_001BA1A0/func_001BA1F0), set the object's state byte to 2 and
// return 1. Otherwise return 0. The 0x700038A0 stores are RAW absolute
// (lui 0x7000 / sw 0x38a0) literals, while the func_001B6F00 argument is a
// symbol reference (&D_700038A0) -> two distinct encodings of the same
// address are required to match. func_001B6F00 takes the original a0 in
// $a0 (kept live), pi in $f12, &D_700038A0 in $a1.
extern float D_700038A0[];
extern int D_00246A20[];
extern void func_001B6F00(void *a, float f, void *b);
extern void func_001BA1A0(int a, void *b);
extern void func_001BA1F0(int a);

int func_00159FC0(unsigned char *a0, int a1) {
    if (*(unsigned char *)(a0 + 0xB) & 4) {
        *(float *)0x700038A0 = 0.3f;
        *(float *)0x700038A4 = 0.0f;
        *(float *)0x700038A8 = 5.0f;
        *(float *)0x700038AC = 1.0f;
        func_001B6F00(a0, 3.14159265f, D_700038A0);
        func_001BA1A0(a1, D_00246A20);
        func_001BA1F0((int)a0);
        *a0 = 2;
        return 1;
    }
    return 0;
}
