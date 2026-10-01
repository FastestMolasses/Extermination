// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA19 overlay, runtime 0x00825EE0 (splat/link name 00825EA0; overlay code
//  is linked 0x40 below where it runs), 0x21C bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA19; lane A13C).
// Role: sub 0 placement [47]: a child func_001C5570(self, (1, 0, 0, 0.25),
//  0x23, 0) in +0x1F0 (0x24 and model 0x1F when D_00810776 == 0xFF at load);
//  in state 1, once D_00810776 == 0xFF or D_008107F6 > 5, model
//  func_001C6120(D_0028A59C, 0x1F), state 2 and the child replaced by a 0x24
//  one. The +0x4C method when func_001B17A0 is set.
typedef void (*ActorFn)(unsigned char *);
extern unsigned char D_00810776;
extern int D_0028A59C;
extern float D_700038A0[];
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern unsigned char *func_001C5570(unsigned char *self, void *v, int a, int b);
extern int func_001C6120(int a, int b);
extern void func_001CA6E0(unsigned char *self, int v);
extern void func_001C62C0(unsigned char *self);
extern int func_001B17A0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA19_00825EA0(unsigned char *self) {
    unsigned char *o;
    switch (self[4]) {
    case 0:
        *(float *)0x700038A0 = 1.0f;
        *(float *)0x700038A4 = 0.0f;
        *(float *)0x700038A8 = 0.0f;
        *(float *)0x700038AC = 0.25f;
        if (*(unsigned char *)0x810776 == 0xFF) {
            self[0xD] = 0x1F;
            if (func_001B0FD0(self) == 0) {
                func_001C6380(self);
                self[4] = 2;
                *(unsigned char **)(self + 0x1F0) = func_001C5570(self, D_700038A0, 0x24, 0);
            }
        } else if (func_001B0FD0(self) == 0) {
            func_001C6380(self);
            self[4] = 1;
            *(unsigned char **)(self + 0x1F0) = func_001C5570(self, D_700038A0, 0x23, 0);
        }
        break;
    case 1:
        if (D_00810776 == 0xFF || *(unsigned char *)0x8107F6 > 5) {
            func_001CA6E0(self, func_001C6120(D_0028A59C, 0x1F));
            func_001C62C0(self);
            func_001C6380(self);
            self[4] = 2;
            o = *(unsigned char **)(self + 0x1F0);
            if (o != 0) {
                o[4] = 3;
                *(float *)0x700038A0 = 1.0f;
                *(float *)0x700038A4 = 0.0f;
                *(float *)0x700038A8 = 0.0f;
                *(float *)0x700038AC = 0.25f;
                *(unsigned char **)(self + 0x1F0) = func_001C5570(self, D_700038A0, 0x24, 0);
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
