// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA19 overlay, runtime 0x008255D0 (splat/link name 00825590; overlay code
//  is linked 0x40 below where it runs), 0x1EC bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA19; lane A13C).
// Role: sub 0 placement [43]. State 0: with D_008107F5 bit 0 set, +0xD = 0x25
//  and state 2; else state 1 and a child func_001C5570(self, (0, 1, 0, 0.25),
//  0x1B, 0) in +0x2EC, moved by +0.5 in x and -0.5 in z. State 1: once bit 0
//  is set, model func_001C6120(D_0028A59C, 0x25), state 2, the child's state
//  3 and +0x2EC = 0. States 1/2: the +0x4C method when func_001B17A0 is set;
//  state 3 func_001AFC10.
typedef void (*ActorFn)(unsigned char *);
extern unsigned char D_008107F5;
extern float D_700038A0[];
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern unsigned char *func_001C5570(unsigned char *self, void *v, int a, int b);
extern int func_001C6120(int a, int b);
extern void func_001CA6E0(unsigned char *self, int v);
extern void func_001C62C0(unsigned char *self);
extern int func_001B17A0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA19_00825590(unsigned char *self) {
    int *w;
    unsigned char *o;
    switch (self[4]) {
    case 0:
        if (D_008107F5 & 1) {
            self[0xD] = 0x25;
            if (func_001B0FD0(self) == 0) {
                func_001C6380(self);
                self[4] = 2;
                *(unsigned char **)(self + 0x2EC) = 0;
            }
        } else if (func_001B0FD0(self) == 0) {
            func_001C6380(self);
            self[4] = 1;
            *(float *)0x700038A0 = 0.0f;
            *(float *)0x700038A4 = 1.0f;
            *(float *)0x700038A8 = 0.0f;
            *(float *)0x700038AC = 0.25f;
            *(unsigned char **)(self + 0x2EC) = func_001C5570(self, D_700038A0, 0x1B, 0);
            o = *(unsigned char **)(self + 0x2EC);
            w = (int *)(self + 0x1F0) + 0x3F;
            if (o != 0) {
                *(float *)(o + 0xB0) += 0.5f;
                *(float *)(*(unsigned char **)w + 0xB8) -= 0.5f;
            }
        }
        break;
    case 1:
        if (D_008107F5 & 1) {
            func_001CA6E0(self, func_001C6120(*(int *)0x28A59C, 0x25));
            func_001C62C0(self);
            func_001C6380(self);
            self[4] = 2;
            o = *(unsigned char **)(self + 0x2EC);
            w = (int *)(self + 0x1F0) + 0x3F;
            if (o != 0) {
                o[4] = 3;
                *w = 0;
            }
        }
        if (func_001B17A0(self) != 0) {
            (*(ActorFn *)(self + 0x4C))(self);
        }
        break;
    case 2:
        if (func_001B17A0(self) != 0) {
            (*(ActorFn *)(self + 0x4C))(self);
        }
        break;
    case 3:
        func_001AFC10(self);
        break;
    }
}
