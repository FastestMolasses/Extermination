// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA16 overlay, runtime 0x00827CE0 (splat/link name 00827CA0; overlay code is
// linked 0x40 below where it runs), 0x2B4 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA16; lane A15C).
// Role: state 0: finds the +0x54-th +0x18 ancestor into +0x1F8, +0x1F0 = -(10
//  + ancestor +0x2E0), position = ancestor's +0x118 object +0xC0 lowered by
//  that, +0x68 = +0x60 = 0.8, state 1 once func_001B10B0(self, 0x74, 0x71) is
//  clear (func_001C63E0(self, 5)). State 1 follows the ancestor; when its
//  +0x2EC and +0x2D4 are clear and +0x56 >= 0 spawns the record of the
//  D_0024D820 table (D_00810700 area, D_0024A850 base, +0x56 row, +0xE column)
//  unless func_001B11E0 says it exists, then state 3; func_001C68C0 and +0x4C
//  when func_001B17A0. State 3/other func_001AFC10.
extern unsigned char D_00810700;
extern unsigned char D_00810701;
extern short D_0024A850[];
extern unsigned char **D_0024D820[];
extern void func_00102948(void *dst, void *src);
extern int func_001B11E0(int id);
extern unsigned char *func_001AFA90(int kind);
extern int func_001B10B0(unsigned char *self, int a, int b);
extern void func_001C63E0(unsigned char *self, int a);
extern void func_001C68C0(unsigned char *self);
extern int func_001B17A0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA16_00827CA0(unsigned char *self) {
    int idx;
    int d;
    int k;
    int i;
    unsigned char *o;
    unsigned char *e;
    unsigned char *p;
    int *w;
    switch (self[4]) {
    case 0:
        o = self;
        for (i = 0; i < *(short *)(self + 0x54); i++) {
            o = *(unsigned char **)(o + 0x18);
        }
        *(unsigned char **)(self + 0x1F8) = o;
        *(float *)(self + 0x1F0) = -(10.0f + *(float *)(*(unsigned char **)(self + 0x1F8) + 0x2E0));
        func_00102948(self + 0xB0, *(unsigned char **)(*(unsigned char **)(self + 0x1F8) + 0x118) + 0xC0);
        *(float *)(self + 0xB4) += *(float *)(self + 0x1F0);
        *(float *)(self + 0x68) = 0.8f;
        *(float *)(self + 0x60) = 0.8f;
        if (func_001B10B0(self, 0x74, 0x71) == 0) {
            func_001C63E0(self, 5);
            self[4] = 1;
        }
        break;
    case 1:
        *(float *)(self + 0xB4) =
            *(float *)(*(unsigned char **)(*(unsigned char **)(self + 0x1F8) + 0x118) + 0xC4) +
            *(float *)(self + 0x1F0);
        p = *(unsigned char **)(self + 0x1F8);
        w = (int *)(p + 0x1F0);
        if (*(int *)(p + 0x2EC) == 0 && w[0x39] == 0) {
            idx = *(short *)(self + 0x56);
            if (idx >= 0) {
                d = D_00810700;
                k = D_0024A850[d];
                if (k == 0) {
                    k++;
                }
                k += idx;
                e = D_0024D820[d][k];
                e += self[0xE] * 44;
                if (func_001B11E0(e[2]) == 0) {
                    o = func_001AFA90(e[4]);
                    if (o != 0) {
                        o[0x9A] = e[2];
                        o[3] = e[6];
                        *(short *)(o + 0x2E) = (*(short *)(e + 6) >> 8) & 0xFF;
                        o[0xD] = e[8];
                        if ((*(short *)(e + 4) & ~0xE0) == 2) {
                            o[0x9D] = D_00810701;
                            o[0x9E] = e[0xA];
                        } else {
                            *(unsigned short *)(o + 0xE) = *(unsigned short *)(e + 0xA);
                        }
                        *(short *)(o + 0x54) = *(short *)(e + 0xC);
                        *(short *)(o + 0x56) = *(short *)(e + 0xE);
                        *(float *)(o + 0xB0) = *(float *)(self + 0xB0);
                        *(float *)(o + 0xB4) = *(float *)(self + 0xB4);
                        *(float *)(o + 0xB8) = *(float *)(self + 0xB8);
                        *(float *)(o + 0xC0) = *(float *)(self + 0xC0);
                        *(float *)(o + 0xC4) = *(float *)(self + 0xC4);
                        *(float *)(o + 0xC8) = *(float *)(self + 0xC8);
                        *(int *)(o + 0x10) = *(int *)(e + 0x28);
                    }
                }
            }
            self[4] = 3;
        }
        func_001C68C0(self);
        if (func_001B17A0(self) != 0) {
            (*(void (**)(unsigned char *))(self + 0x4C))(self);
        }
        break;
    case 3:
    default:
        func_001AFC10(self);
        break;
    }
}
