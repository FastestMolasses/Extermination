// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA17 overlay, runtime 0x008243E0 (splat/link name 008243A0; overlay code
//  is linked 0x40 below where it runs), 0x1B4 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA17; lane OVLC).
// Role: sub 0 placement [31]. State 1: flag 0x2D or the end of a +0x28
//  countdown sets state 2, the +0x1C object's state 2 and the model
//  func_001CA6E0(self, func_001C6120(D_0028A59C, 0xD)) + func_001C62C0; the
//  countdown also spawns effect 0x80000013 at +0xB0 + (0, 15, 0). States 1 /
//  2: the +0x4C method when func_001B17A0. State 3 func_001AFC10.
typedef void (*ActorFn)(unsigned char *);
#define S16(o) (*(short *)(self + (o)))
extern int D_0028A59C;
extern float D_700038A0[];
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern int func_001BA1C0(unsigned char *self, int idx);
extern int func_001C6120(int set, int id);
extern void func_001CA6E0(unsigned char *self, int a);
extern void func_001C62C0(unsigned char *self);
extern void func_001EFD20(int id, void *pos);
extern int func_001B17A0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA17_008243A0(unsigned char *self) {
    short n;
    switch (self[4]) {
    case 0:
        if (func_001B0FD0(self) == 0) {
            func_001C6380(self);
        }
        break;
    case 1:
        if (func_001BA1C0(self, 0x2D) != 0) {
            self[4] = 2;
            (*(unsigned char **)(self + 0x1C))[4] = 2;
            func_001CA6E0(self, func_001C6120(D_0028A59C, 0xD));
            func_001C62C0(self);
        }
        n = S16(0x28);
        if (n != 0) {
            S16(0x28) = n - 1;
            if (S16(0x28) == 0) {
                self[4] = 2;
                (*(unsigned char **)(self + 0x1C))[4] = 2;
                D_700038A0[0] = *(float *)(self + 0xB0);
                D_700038A0[1] = 15.0f + *(float *)(self + 0xB4);
                D_700038A0[2] = *(float *)(self + 0xB8);
                D_700038A0[3] = 1.0f;
                func_001EFD20(0x80000013, D_700038A0);
                func_001CA6E0(self, func_001C6120(D_0028A59C, 0xD));
                func_001C62C0(self);
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
