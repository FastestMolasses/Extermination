// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA14 overlay, runtime 0x008238E0 (splat/link name 008238A0; overlay code
//  is linked 0x40 below where it runs), 0x244 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA14; lane A03C).
// Role: sub 0 placement [0]. Copies the 80-byte area 0x826F10 to the stack.
//  State 0: state 3 when func_001BA1C0(self, 0x3D); else func_001B10B0(self,
//  +0xD, 0x5F), func_001C63E0(self, 1), func_001CA6F0(self, 2), +0x58 =
//  *(int *)0x28A610, state 1, +0 = 1. State 1, +5 0: script 0x826C10 (+5 1)
//  when func_001B1EA0(0, D_00810350, area, 5) and D_00810354 < 435. +5 1: at
//  the script end +0x2E = 0xFFFF, func_001C4760(0x16, 1), D_00810815 = 0xFF,
//  func_001FAE70(0), +5 2. func_001B17A0, func_001C68C0 and the +0x4C method
//  run every frame of state 1, except in +5 1 while D_70003B92 is set.
//  States 2 / 3: func_001AFC10.
typedef void (*ActorFn)(unsigned char *);
typedef struct {
    float v[20];
} Area __attribute__((aligned(16)));
extern Area D_overlay_AREA14_00826F10;
extern float D_00810350[];
extern float D_00810354[];
extern unsigned char D_00810815[];
extern unsigned char D_70003B92[];
extern char D_overlay_AREA14_00826C10[];
extern int func_001BA1C0(unsigned char *self, int idx);
extern void func_001B10B0(unsigned char *self, int a, int b);
extern void func_001C63E0(unsigned char *self, int a);
extern void func_001CA6F0(unsigned char *self, int a);
extern int func_001B1EA0(int a, void *b, void *c, int d);
extern void func_001BA1A0(unsigned char *talk, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_001C4760(int id, int on);
extern void func_001FAE70(int a);
extern void func_001B17A0(unsigned char *self);
extern void func_001C68C0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA14_008238A0(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    Area area = D_overlay_AREA14_00826F10;
    switch (self[4]) {
    case 0:
        if (func_001BA1C0(self, 0x3D) != 0) {
            self[4] = 3;
            break;
        }
        func_001B10B0(self, self[0xD], 0x5F);
        func_001C63E0(self, 1);
        func_001CA6F0(self, 2);
        *(int *)(self + 0x58) = *(int *)0x28A610;
        self[4] = 1;
        self[0] = 1;
        break;
    case 1:
        switch (self[5]) {
        case 0:
            if (func_001B1EA0(0, D_00810350, &area, 5) != 0) {
                if (D_00810354[0] < 435.0f) {
                    func_001BA1A0(talk, D_overlay_AREA14_00826C10);
                    self[5] = 1;
                }
            }
            func_001B17A0(self);
            func_001C68C0(self);
            (*(ActorFn *)(self + 0x4C))(self);
            break;
        case 1:
            if (func_001BA1F0(self) != 0) {
                *(unsigned short *)(self + 0x2E) = 0xFFFF;
                func_001C4760(0x16, 1);
                D_00810815[0] = 0xFF;
                func_001FAE70(0);
                self[5] = 2;
            }
            if (D_70003B92[0] == 0) {
                func_001B17A0(self);
                func_001C68C0(self);
                (*(ActorFn *)(self + 0x4C))(self);
            }
            break;
        case 2:
            func_001B17A0(self);
            func_001C68C0(self);
            (*(ActorFn *)(self + 0x4C))(self);
            break;
        }
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
