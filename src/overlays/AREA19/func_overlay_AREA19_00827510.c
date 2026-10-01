// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA19 overlay, runtime 0x00827550 (splat/link name 00827510; overlay code
//  is linked 0x40 below where it runs), 0x234 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA19; lane A13C).
// Role: sub 1 placement [53]. State 0: state 3 when func_001BA1C0(self, 0x26)
//  is set or (0x25) is clear; else func_001B10B0(self, +0xD, 0x97),
//  func_001C63E0(self, 0), +0x58 = D_0028A704, func_001F1110(self, 3), state
//  1. +5 0: with 445 <= y <= 460: func_001F1180(self) and, in the area
//  0x82F820, func_001FB0B0(0) and script 0x82D590; +5 1: func_001F1180 for
//  300 frames, func_001C47A0(0x24, 1) when the script ends. Then
//  func_001C64F0, func_001C68C0, func_001B17A0 and the +0x4C method.
typedef void (*ActorFn)(unsigned char *);
extern float D_00810350[];
extern float D_00810354;
extern char D_overlay_AREA19_0082D590[];
extern char D_overlay_AREA19_0082F820[];
extern int func_001BA1C0(unsigned char *self, int idx);
extern void func_001B10B0(unsigned char *self, int a, int b);
extern void func_001C63E0(unsigned char *self, int a);
extern void func_001F1110(unsigned char *self, int a);
extern void func_001F1180(unsigned char *self);
extern int func_001B1EA0(int a, void *b, void *c, int d);
extern void func_001FB0B0(int a);
extern void func_001BA1A0(unsigned char *talk, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_001C47A0(int id, int on);
extern void func_001C64F0(unsigned char *self, float step);
extern void func_001C68C0(unsigned char *self);
extern void func_001B17A0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA19_00827510(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    float y;
    switch (self[4]) {
    case 0:
        if (func_001BA1C0(self, 0x26) != 0) {
            self[4] = 3;
            break;
        }
        if (func_001BA1C0(self, 0x25) == 0) {
            self[4] = 3;
            break;
        }
        func_001B10B0(self, self[0xD], 0x97);
        func_001C63E0(self, 0);
        *(int *)(self + 0x58) = *(int *)0x28A704;
        func_001F1110(self, 3);
        self[0] = 1;
        self[4] = 1;
        *(short *)(self + 0x28) = 0;
        break;
    case 1:
        switch (self[5]) {
        case 0:
            y = D_00810354;
            if (!(y < 445.0f) && y <= 460.0f) {
                if (func_001B1EA0(0, D_00810350, (void *)D_overlay_AREA19_0082F820, 4) != 0) {
                    func_001FB0B0(0);
                    func_001BA1A0(talk, D_overlay_AREA19_0082D590);
                    self[5] = 1;
                }
                func_001F1180(self);
            }
            break;
        case 1:
            *(short *)(self + 0x28) += 1;
            if (*(short *)(self + 0x28) < 300) {
                func_001F1180(self);
            }
            if (func_001BA1F0(self) != 0) {
                func_001C47A0(0x24, 1);
                self[5] = 2;
            }
            break;
        case 2:
            break;
        }
        func_001C64F0(self, 1.0f);
        func_001C68C0(self);
        func_001B17A0(self);
        (*(ActorFn *)(self + 0x4C))(self);
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
