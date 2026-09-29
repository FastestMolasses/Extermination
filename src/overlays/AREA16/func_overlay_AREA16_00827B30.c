// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA16 overlay, runtime 0x00827B70 (splat/link name 00827B30; overlay code is
// linked 0x40 below where it runs), 0x164 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA16; lane A15C).
// Role: state 0 (after func_001B0FD0): func_001C6380 and a class 8 child (+0xD
//  0x14, +0xE 0xFF00, 0x828200 behaviour) at the position lowered by +0x2E0.
//  State 1: +0x28 counter; past 200 a 50% chance of sound 0x451 and reset;
//  func_001B1B70 and +0x4C. States 2/3 func_001AFC10.
extern char D_overlay_AREA16_00828200[];
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern unsigned char *func_001AFA90(int kind);
extern int func_00122BB8(void);
extern void func_001FBD50(unsigned char *self, int id, int a2, float f12);
extern void func_001B1B70(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA16_00827B30(unsigned char *self) {
    unsigned char *o;
    float d;
    switch (self[4]) {
    case 0:
        if (func_001B0FD0(self) == 0) {
            func_001C6380(self);
            o = func_001AFA90(8);
            if (o != 0) {
                o[3] = 0;
                *(short *)(o + 0x2E) = 0;
                o[0xD] = 0x14;
                *(unsigned short *)(o + 0xE) = 0xFF00;
                *(short *)(o + 0x56) = 0;
                *(short *)(o + 0x54) = 0;
                *(float *)(o + 0xB0) = *(float *)(self + 0xB0);
                /* staged subtraction (loads +0x2E0 first) */
                d = *(float *)(self + 0x2E0);
                d = *(float *)(self + 0xB4) - d;
                *(float *)(o + 0xB4) = d;
                *(float *)(o + 0xB8) = *(float *)(self + 0xB8);
                *(float *)(o + 0xC0) = *(float *)(self + 0xC0);
                *(float *)(o + 0xC4) = *(float *)(self + 0xC4);
                *(float *)(o + 0xC8) = *(float *)(self + 0xC8);
                *(char **)(o + 0x10) = D_overlay_AREA16_00828200;
            }
        }
        break;
    case 1:
        (*(short *)(self + 0x28))++;
        if (*(short *)(self + 0x28) > 200) {
            if (((func_00122BB8() >> 16) * 2 >> 15) != 0) {
                func_001FBD50(self, 0x451, 0, 500.0f);
            }
            *(short *)(self + 0x28) = 0;
        }
        func_001B1B70(self);
        (*(void (**)(unsigned char *))(self + 0x4C))(self);
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
