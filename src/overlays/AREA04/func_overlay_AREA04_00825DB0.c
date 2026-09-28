// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA04 overlay, runtime 0x00825DF0 (splat/link name 00825DB0;
// overlay code is linked 0x40 below where it runs), 0x2D0 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA04; lane A04C).
// Role: state 0 (func_001B0FD0 == 0): state 4 with +0x28 = +0x2A = 0 when
// D_0081083B, else state 1; +0 = 2, +0x30 = 0x82C630, func_001C6380;
// spawns func_001C5570(self, (0, 1, 0, 0.25), 0xC, 0) into +0x2EC and
// moves it by (+0.7, -0.1) on +0xB0/+0xB4. State 1: +0 = 1 when
// D_00810702 == 4 else 2; func_001B17A0; +0xB bit 2 starts script
// 0x82C3B0, state 4, link +0x28 = 120, +0x28 = 120, +0x2A = 60. State 4: at
// script end +0x28 = 1, +0x2A = 0, (+0x1C)[4] = 100, D_0081083B = 0xFF;
// +0x28 counting to 0 plays func_001FBD50(self, 0x19A, 0, 300), sets the
// spawned object's +4 = 3 and clears +0x2EC; else +0x2A counting to 0
// plays 0x455, D_0081083B = 0xFF, (+0x1C)[4] = 1. States 1/4 run the
// +0x4C method; others func_001AFC10.
extern unsigned char D_0081083B;
extern unsigned char D_00810702;
extern float D_700038A0[4];
extern char D_overlay_AREA04_0082C630[];
extern char D_overlay_AREA04_0082C3B0[];
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern char *func_001C5570(unsigned char *self, float *pos, int kind, int a3);
extern int func_001B17A0(unsigned char *self);
extern void func_001BA1A0(void *who, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_001FBD50(unsigned char *e, int id, int b, float f);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA04_00825DB0(unsigned char *self) {
    unsigned char *blk = self + 0x1F0;
    char **fx;
    switch (self[4]) {
    case 0:
        if (func_001B0FD0(self) == 0) {
            if (D_0081083B != 0) {
                self[4] = 4;
                *(short *)(self + 0x2A) = 0;
                *(short *)(self + 0x28) = 0;
            } else {
                self[4] = 1;
            }
            self[0] = 2;
            *(char **)(self + 0x30) = D_overlay_AREA04_0082C630;
            func_001C6380(self);
            D_700038A0[0] = 0.0f;
            D_700038A0[1] = 1.0f;
            D_700038A0[2] = 0.0f;
            D_700038A0[3] = 0.25f;
            *(char **)(self + 0x2EC) = func_001C5570(self, D_700038A0, 0xC, 0);
            *(float *)(*(char **)(self + 0x2EC) + 0xB0) += 0.7f;
            *(float *)(*(char **)(self + 0x2EC) + 0xB4) -= 0.1f;
        }
        break;
    case 1:
        if (D_00810702 == 4) {
            self[0] = 1;
        } else {
            self[0] = 2;
        }
        func_001B17A0(self);
        if (self[0xB] & 4) {
            func_001BA1A0(blk, D_overlay_AREA04_0082C3B0);
            self[4] = 4;
            func_001BA1F0(self);
            *(short *)(*(char **)(self + 0x18) + 0x28) = 0x78;
            *(short *)(self + 0x28) = 0x78;
            *(short *)(self + 0x2A) = 0x3C;
        }
        (*(void (**)(unsigned char *))(self + 0x4C))(self);
        break;
    case 4:
        if (self[0xB] != 0 && func_001BA1F0(self) != 0) {
            self[0xB] = 0;
            *(short *)(self + 0x28) = 1;
            *(short *)(self + 0x2A) = 0;
            (*(unsigned char **)(self + 0x1C))[4] = 0x64;
            D_0081083B = 0xFF;
        }
        if (*(short *)(self + 0x28) != 0) {
            *(short *)(self + 0x28) -= 1;
            if (*(short *)(self + 0x28) == 0) {
                fx = (char **)((int *)(self + 0x1F0) + 0x3F);
                if (*fx != 0) {
                    func_001FBD50(self, 0x19A, 0, 300.0f);
                    (*fx)[4] = 3;
                    *fx = 0;
                }
            }
        } else if (*(short *)(self + 0x2A) != 0) {
            *(short *)(self + 0x2A) -= 1;
            if (*(short *)(self + 0x2A) == 0) {
                func_001FBD50(self, 0x455, 0, 300.0f);
                D_0081083B = 0xFF;
                (*(unsigned char **)(self + 0x1C))[4] = 1;
            }
        }
        (*(void (**)(unsigned char *))(self + 0x4C))(self);
        break;
    case 3:
    default:
        func_001AFC10(self);
        break;
    }
}
