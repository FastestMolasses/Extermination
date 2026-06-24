// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
//
// Object/entity initialiser. arg0 (s0) is the entity record, arg1 (s1) a
// source/config struct. Stamps a block of float defaults into the config
// struct (1.0f at +0xEC/+0x6C/+0x78/+0x7C/+0x84/+0x8C, -0.4f at +0x64, zeros
// elsewhere), runs the matrix/transform init func_001029C0(record+0xD0),
// stores the address of the gp-rel constant D_00275668 into record+0x30,
// then selects a mode via func_00128390(record, flag) where flag is 1 iff
// the config byte at +0xE1 is nonzero, storing the result short at +0x34.
// Finishes by clearing several status fields and writing four 1.0f values
// into record+0x80..0x8C.
//
// mwcc 2.3.3 matches byte-identical; the pinned 991202 build caps at 83.8%
// (clean-store delay-slot scheduling: it reschedules the +0x80..0x8C store
// burst around the func_001029C0 call). D_00275668 is gp-rel (sdatathreshold
// 8 -> %gp_rel addiu-from-gp); D_0028A4C8 is sized [64] so it stays hi/lo.
extern void func_001029C0(void *p);
extern int func_00128390(int a, int b);
extern int D_00275668;
extern void *D_0028A4C8[64];

void func_001289C0(int a0, int a1) {
    char *s0 = (char *)a0;
    char *s1 = (char *)a1;

    *(float *)(s1 + 0xEC) = 1.0f;
    *(int *)(s1 + 0xD8) = 0;
    *(int *)(s1 + 0xE4) = 0;
    *(int *)(s1 + 0xDC) = 0;
    *(int *)(s1 + 0x60) = 0;
    *(float *)(s1 + 0x64) = -0.4f;
    *(int *)(s1 + 0x68) = 0;
    *(float *)(s1 + 0x6C) = 1.0f;
    *(int *)(s1 + 0x70) = 0;
    *(int *)(s1 + 0x74) = 0;
    *(float *)(s1 + 0x78) = 1.0f;
    *(float *)(s1 + 0x7C) = 1.0f;
    *(int *)(s1 + 0x80) = 0;
    *(float *)(s1 + 0x84) = 1.0f;
    *(int *)(s1 + 0x88) = 0;
    *(float *)(s1 + 0x8C) = 1.0f;
    func_001029C0(s0 + 0xD0);
    *(int *)(s0 + 0x30) = (int)&D_00275668;
    *(char *)(s0 + 0xB) = 0;
    if (*(unsigned char *)(s1 + 0xE1) == 0) {
        *(short *)(s0 + 0x34) = func_00128390((int)s0, 0);
    } else {
        *(short *)(s0 + 0x34) = func_00128390((int)s0, 1);
    }
    *(short *)(s0 + 0x36) = 0;
    *(char *)(s1 + 0xFA) = 0;
    *(char *)(s1 + 0xFB) = 0;
    *(short *)(s1 + 0xF6) = 0;
    *(int *)(s0 + 0x58) = (int)D_0028A4C8[0];
    *(float *)(s0 + 0x80) = 1.0f;
    *(float *)(s0 + 0x84) = 1.0f;
    *(float *)(s0 + 0x88) = 1.0f;
    *(float *)(s0 + 0x8C) = 1.0f;
    *(char *)(s0 + 0xA) = 0;
}
