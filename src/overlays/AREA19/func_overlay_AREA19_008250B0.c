// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA19 overlay, runtime 0x008250F0 (splat/link name 008250B0; overlay code
//  is linked 0x40 below where it runs), 0x150 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA19; lane A13C).
// Role: sub 0 placement [6]. State 0: state 3 when func_001BA1C0(self, 0x1D)
//  is set; else func_001B10B0(self, +0xD, 0x67), func_001C63E0(self, 2) (0
//  when D_00810775 != 0), func_001CA6F0(self, 2), state 1, +0 = 1. State 1 by
//  D_008107F5: bit 0 clear -> 0x825240, else bit 2 clear -> 0x825420, else
//  func_001C64F0, func_001B17A0, func_001C68C0 and the +0x4C method. States
//  2/3 func_001AFC10.
typedef void (*ActorFn)(unsigned char *);
extern unsigned char D_008107F5;
extern int func_001BA1C0(unsigned char *self, int idx);
extern void func_001B10B0(unsigned char *self, int a, int b);
extern void func_001C63E0(unsigned char *self, int a);
extern void func_001CA6F0(unsigned char *self, int a);
extern void func_001C64F0(unsigned char *self, float step);
extern void func_001B17A0(unsigned char *self);
extern void func_001C68C0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);
extern void func_overlay_AREA19_00825240(unsigned char *self);
extern void func_overlay_AREA19_00825420(unsigned char *self);

void func_overlay_AREA19_008250B0(unsigned char *self) {
    int f;
    switch (self[4]) {
    case 0:
        if (func_001BA1C0(self, 0x1D) != 0) {
            self[4] = 3;
            break;
        }
        func_001B10B0(self, self[0xD], 0x67);
        if (*(unsigned char *)0x810775 == 0) {
            func_001C63E0(self, 2);
        } else {
            func_001C63E0(self, 0);
        }
        func_001CA6F0(self, 2);
        self[4] = 1;
        self[0] = 1;
        break;
    case 1:
        f = D_008107F5;
        if (!(f & 1)) {
            func_overlay_AREA19_00825240(self);
        } else if (!(f & 4)) {
            func_overlay_AREA19_00825420(self);
        } else {
            func_001C64F0(self, 1.0f);
            func_001B17A0(self);
            func_001C68C0(self);
            (*(ActorFn *)(self + 0x4C))(self);
        }
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
