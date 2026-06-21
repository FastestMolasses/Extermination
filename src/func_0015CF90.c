// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// SEMANTICS: snapshots actor flags +0x235/+0x234 into globals
// D_00810706/D_00810707 and positions +0x220/+0x228 into
// D_00810858/D_0081085C; if +0x220 <= 0.0 and the latch D_008106B9 is
// clear, set it to 1.
//
// idiom-13 clean-store delay-slot wall: 991202 fills the bc1f/bnez
// delay slots from the safe fall-through (lui %hi / li 1); CW 2.3.1
// (and mwcc 2.3.3) leave the nops -> 2.3.3 matches byte-for-byte.
extern unsigned char D_00810706, D_00810707, D_008106B9;
extern float D_00810858, D_0081085C;

void func_0015CF90(unsigned char *a0) {
    D_00810706 = a0[0x235];
    D_00810707 = a0[0x234];
    D_00810858 = *(float *)(a0 + 0x220);
    D_0081085C = *(float *)(a0 + 0x228);
    if (*(float *)(a0 + 0x220) <= 0.0f)
        if (D_008106B9 == 0) D_008106B9 = 1;
}
