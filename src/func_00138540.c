// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
//
// State-machine entry/init for an animated entity. arg0 (p) is the entity,
// arg1 (q) a paired state block. Bumps the p[4] counter, mirrors p[0xD] flags
// into q[0x88], and zeroes several work fields. If the global gate D_00810808
// == 0xFF, sets bit 0x80 in p[0xD]. Two init paths keyed on that bit: the
// "boosted" path (0x96/0x50 timer into p[0x34], seeds the 0x60..0x6C scale
// quad, clip 0x7B/0x7C) and the normal path (0x3C/0x28 timer, clip 0x79/0x7C);
// the secondary timer value is gated by D_0081070A. If func_001B10B0 reports
// busy, returns early. Otherwise finishes init: marks active, installs the
// gp-base default table &D_002753B0 at p[0x30], copies two matrices via
// func_00102948, seeds constants, and computes q[0x20] from a global base plus
// a randomized 4-bit nibble (func_00122BB8). -sdatathreshold 8 makes the int
// D_002753B0 gp-relative (CW's addiu v0, gp, %gp_rel form) while the >8-byte
// extern arrays stay absolute; the p[0xD] OR-store reads unsigned (lbu) so the
// flag test reuses the zero-extended value. mwcc 2.3.3 only; pinned 991202
// reaches 75.3%. objdiff 100.0 vs expected.
extern int bone_init_default_2(char *p, int n);
extern void func_00102948(char *dst, char *src);
extern int func_00122BB8(void);
extern int func_001B10B0(char *p, int a, int b);
extern int D_002753B0;
extern int D_0028A678[];
extern unsigned char D_0081070A[];
extern unsigned char D_00810808[];

void func_00138540(char *p, char *q) {
    *(unsigned char *)(p + 4) = *(unsigned char *)(p + 4) + 1;
    *(char *)(q + 0x88) = *(char *)(p + 0xD);
    *(char *)(q + 0x87) = 1;
    *(int *)(p + 0x20) = 0;
    *(int *)(p + 0xC0) = 0;
    *(int *)(p + 0xC8) = 0;
    if (D_00810808[0] == 0xFF) {
        *(unsigned char *)(p + 0xD) = *(unsigned char *)(p + 0xD) | 0x80;
    }
    if (*(unsigned char *)(p + 0xD) & 0x80) {
        if (D_0081070A[0] != 0) {
            *(short *)(p + 0x34) = 0x96;
        } else {
            *(short *)(p + 0x34) = 0x50;
        }
        *(int *)(p + 0x60) = 0x3FA66666;
        *(int *)(p + 0x64) = 0x3FA66666;
        *(int *)(p + 0x68) = 0x3FA66666;
        *(int *)(p + 0x6C) = 0x3F800000;
        if (func_001B10B0(p, 0x7B, 0x7C) != 0) {
            return;
        }
    } else {
        if (D_0081070A[0] != 0) {
            *(short *)(p + 0x34) = 0x3C;
        } else {
            *(short *)(p + 0x34) = 0x28;
        }
        if (func_001B10B0(p, 0x79, 0x7C) != 0) {
            return;
        }
    }
    bone_init_default_2(p, 1);
    *(char *)(p + 0) = 1;
    *(char **)(p + 0x30) = (char *)&D_002753B0;
    *(char *)(p + 0x5E) = 0;
    *(int *)(p + 0x58) = D_0028A678[0];
    func_00102948(q, p + 0xB0);
    func_00102948(q + 0x10, p + 0xC0);
    *(char *)(q + 0x82) = 0;
    *(int *)(q + 0x4C) = 0x3ECCCCCD;
    *(int *)(q + 0x54) = 0x3D32B8C3;
    *(short *)(q + 0x20) = *(short *)0x70003B8A + ((func_00122BB8() >> 0xB) & 0xF);
}
