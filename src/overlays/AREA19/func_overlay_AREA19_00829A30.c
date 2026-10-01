// NEARMISS func_overlay_AREA19_00829A30 (99.53%, mwcc 2.3.3; linked from its splat .s)
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// AREA19 overlay, runtime 0x00829A70 (splat/link name 00829A30; overlay code
//  is linked 0x40 below where it runs), 0x320 bytes.
// Role (read from the instructions): sub 1 placement [46]. State 0 when
//  func_001B0FD0 returns 0: with D_00810838 != 0 func_0019C6F0(5, 0), the
//  +0x74 floats of the objects at *(D_00275B40 + 8) / *(D_00275B40 + 0xC) =
//  -pi/2 / +pi/2, +2 = 4, state 2; otherwise +8 = 1, +0x2EC = +0xE, +0xE |=
//  0xFF00, +0x30 = 0x82F790 and a child func_001AFA90(0xC) (model 0x18,
//  behaviour D_001C5760) in +0x2E8. State 1: on +0xB bit 2 with bit 0:
//  D_00810838 = 1, func_001C47E0(0x25, 1), +0xE restored, the same
//  func_0019C6F0 / turn, the child's state 3, +2 = 4, state 2, D_70003B8D /
//  B91 / B92 = 0 and, when D_70003B8F == 2, func_001CA770(D_008102B0) and
//  D_70003B8F = 1; without bit 0 script 0x82F690 (+5 1 clears +0xB when it
//  ends). States 1/2: func_001B1B70 and the +0x4C method.
// Divergence: the spawned child (func_001AFA90(0xC)) lives in s0 and the
//  +0x2E8 slot pointer in s1 in the original; mwcc 2.3.3 swaps the two.
//  Declaration orders and a separate child local were tried.
typedef void (*ActorFn)(unsigned char *);
extern char *D_00275B40;
extern unsigned char D_00810838[];
extern char D_008102B0[];
extern char D_001C5760[];
extern char D_overlay_AREA19_0082F690[];
extern char D_overlay_AREA19_0082F790[];
extern int func_001B0FD0(unsigned char *self);
extern void func_0019C6F0(int id, int on);
extern void func_001C6380(unsigned char *self);
extern unsigned char *func_001AFA90(int cls);
extern void func_00102948(void *dst, void *src);
extern void func_001C47E0(int id, int on);
extern void func_001CA770(void *p);
extern void func_001BA1A0(unsigned char *talk, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_001B1B70(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA19_00829A30(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    int *w;
    unsigned char *o;
    unsigned char *c;
    int b;
    switch (self[4]) {
    case 0:
        if (func_001B0FD0(self) == 0) {
            self[0] = 1;
            *(unsigned char **)(self + 0x2E8) = 0;
            w = (int *)(self + 0x1F0) + 0x3E;
            if (D_00810838[0] != 0) {
                func_0019C6F0(5, 0);
                *(float *)(*(char **)(D_00275B40 + 8) + 0x74) = -1.5707964f;
                *(float *)(*(char **)(D_00275B40 + 0xC) + 0x74) = 1.5707964f;
                func_001C6380(self);
                self[2] = 4;
                self[4] = 2;
                break;
            }
            self[8] = 1;
            *(int *)(self + 0x2EC) = *(unsigned short *)(self + 0xE);
            *(unsigned short *)(self + 0xE) |= 0xFF00;
            *(void **)(self + 0x30) = D_overlay_AREA19_0082F790;
            func_001C6380(self);
            if (D_00810838[0] == 0) {
                c = func_001AFA90(0xC);
                if (c != 0) {
                    c[0x9A] = 0;
                    c[3] = 0;
                    *(short *)(c + 0x2E) = 0;
                    c[0xD] = 0x18;
                    *(unsigned short *)(c + 0xE) = 0xFFFF;
                    *(short *)(c + 0x54) = 0;
                    *(short *)(c + 0x56) = 0;
                    *(float *)(c + 0xA0) = 1.0f;
                    *(float *)(c + 0xA4) = 0.0f;
                    *(float *)(c + 0xA8) = 0.0f;
                    *(float *)(c + 0xAC) = 0.25f;
                    func_00102948(c + 0xB0, self + 0xB0);
                    func_00102948(c + 0xC0, self + 0xC0);
                    *(char **)(c + 0x10) = D_001C5760;
                    *(unsigned char **)w = c;
                }
            }
        }
        break;
    case 1:
        switch (self[5]) {
        case 0:
            b = self[0xB];
            if (b & 4) {
                if (b & 1) {
                    D_00810838[0] = 1;
                    func_001C47E0(0x25, 1);
                    *(unsigned short *)(self + 0xE) = *(int *)(self + 0x2EC);
                    func_0019C6F0(5, 0);
                    *(float *)(*(char **)(D_00275B40 + 8) + 0x74) = -1.5707964f;
                    *(float *)(*(char **)(D_00275B40 + 0xC) + 0x74) = 1.5707964f;
                    o = *(unsigned char **)(self + 0x2E8);
                    if (o != 0) {
                        o[4] = 3;
                    }
                    func_001C6380(self);
                    self[2] = 4;
                    self[4] = 2;
                    *(unsigned char *)0x70003B8D = 0;
                    *(unsigned char *)0x70003B91 = 0;
                    *(unsigned char *)0x70003B92 = 0;
                    if (*(unsigned char *)0x70003B8F == 2) {
                        func_001CA770(D_008102B0);
                        *(unsigned char *)0x70003B8F = 1;
                    }
                } else {
                    func_001BA1A0(talk, D_overlay_AREA19_0082F690);
                    self[5]++;
                }
            }
            break;
        case 1:
            if (func_001BA1F0(self) != 0) {
                self[5] = 0;
                self[0xB] = 0;
            }
            break;
        }
        func_001B1B70(self);
        (*(ActorFn *)(self + 0x4C))(self);
        break;
    case 2:
        func_001B1B70(self);
        (*(ActorFn *)(self + 0x4C))(self);
        break;
    case 3:
    default:
        func_001AFC10(self);
        break;
    }
}
