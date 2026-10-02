// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA17 overlay, runtime 0x00825EA0 (splat/link name 00825E60; overlay code
//  is linked 0x40 below where it runs), 0x214 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA17; lane OVLC).
// Role: sub 0 placement [36] (counter 0x2F). State 0: +0 = 2 with flag 0x2F
//  else 1, +0x30 = 0x828E30, a child func_001C5570(self, (0, 1, 0, 0.25),
//  0x10, 0) in +0x2EC lowered by 0.4. State 1: +5 0 on +0xB bit 2 +0 = 2,
//  D_00810807 = 2; D_00810807 == 3 starts a +0x28 count (+5 1). While
//  counting (or with D_00810787 == 0xFF), past 139 frames or with D_00810787
//  == 0xFF the child gets state 3 and +0x2EC = 0. While D_00810807 != 3
//  func_001C6380, func_001B17A0 and the +0x4C method. States 2 / 3
//  func_001AFC10.
typedef void (*ActorFn)(unsigned char *);
#define S16(o) (*(short *)(self + (o)))
extern unsigned char D_00810787;
extern unsigned char D_00810807;
extern float D_700038A0[];
extern char D_overlay_AREA17_00828E30[];
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern int func_001BA1C0(unsigned char *self, int idx);
extern unsigned char *func_001C5570(unsigned char *self, void *v, int a2, int a3);
extern int func_001B17A0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA17_00825E60(unsigned char *self) {
    unsigned char *o;
    int *hp;
    switch (self[4]) {
    case 0:
        if (func_001B0FD0(self) != 0) {
            break;
        }
        func_001C6380(self);
        if (func_001BA1C0(self, 0x2F) != 0) {
            self[0] = 2;
        } else {
            self[0] = 1;
        }
        *(char **)(self + 0x30) = D_overlay_AREA17_00828E30;
        D_700038A0[0] = 0.0f;
        D_700038A0[1] = 1.0f;
        D_700038A0[2] = 0.0f;
        D_700038A0[3] = 0.25f;
        *(unsigned char **)(self + 0x2EC) = func_001C5570(self, D_700038A0, 0x10, 0);
        *(float *)(*(unsigned char **)(self + 0x2EC) + 0xB4) -= 0.4f;
        break;
    case 1:
        switch (self[5]) {
        case 0:
            if (self[0xB] & 4) {
                self[0] = 2;
                self[0xB] = 0;
                D_00810807 = 2;
            }
            if (D_00810807 == 3) {
                self[5] = 1;
                S16(0x28) = 1;
            }
            break;
        case 1:
            break;
        }
        if (S16(0x28) != 0 || D_00810787 == 0xFF) {
            S16(0x28)++;
            if (S16(0x28) > 139 || D_00810787 == 0xFF) {
                o = *(unsigned char **)(self + 0x2EC);
                hp = (int *)(self + 0x1F0) + 0x3F;
                if (o != 0) {
                    o[4] = 3;
                    *hp = 0;
                }
                S16(0x28) = 0;
            }
        }
        if (D_00810807 != 3) {
            func_001C6380(self);
            func_001B17A0(self);
            (*(ActorFn *)(self + 0x4C))(self);
        }
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
