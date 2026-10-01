// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA14 overlay, runtime 0x00823B30 (splat/link name 00823AF0; overlay code
//  is linked 0x40 below where it runs), 0x300 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA14; lane A03C).
// Role: sub 0 placement [2]. State 0: func_001B10B0(self, +0xD, 0x56),
//  func_001C63E0(self, 0), func_001CA6F0(self, 2), func_001BA8E0(self, +0xD),
//  +0 = 1, +0x30 = 0x827480, +0x58 = *(int *)0x28A5EC, state 1. State 1:
//  state 3 when func_001BA1C0(self, 0x3F). While D_00810816 is 0: +5 0 with
//  D_00810354 >= 675: script 0x826FC0 (+5 1) inside the area 0x828400
//  (func_001B1EA0 kind 4), func_001BA580(self, +0xD), func_001C64F0(self,
//  1.0); +5 1: at the script end D_00810816 = 1, +0x2E = 0xFFFF, +0x40 =
//  *(int *)0x28A5E8, func_001C67E0(self, 0, 0, 0), +5 0, func_001C47A0(0xF,
//  1), func_001C4760(0x17, 1), func_001FAE70(0); then func_001BA580 and
//  +0x1FE = func_001C64F0(self, 0.5). Once D_00810816 is set: +0xB bit 2
//  starts script 0x827340, whose end calls func_001C67E0(self, 0, 20.0, 0.0)
//  and clears +0xB and +5; func_001BA580, func_001C64F0(self, 1.0). Then
//  func_001B17A0, +1 = 1, func_001C68C0, the +0x4C method. States 2 / 3:
//  func_001BA540 and func_001AFC10.
typedef void (*ActorFn)(unsigned char *);
extern float D_00810350[];
extern float D_00810354[];
extern unsigned char D_00810816[];
extern char D_overlay_AREA14_00827480[];
extern char D_overlay_AREA14_00828400[];
extern char D_overlay_AREA14_00826FC0[];
extern char D_overlay_AREA14_00827340[];
extern void func_001B10B0(unsigned char *self, int a, int b);
extern void func_001C63E0(unsigned char *self, int a);
extern void func_001CA6F0(unsigned char *self, int a);
extern void func_001BA8E0(unsigned char *self, int a);
extern int func_001BA1C0(unsigned char *self, int idx);
extern int func_001B1EA0(int a, void *b, void *c, int d);
extern void func_001BA1A0(unsigned char *talk, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_001BA580(unsigned char *self, int a1);
extern short func_001C64F0(unsigned char *self, float step);
extern void func_001C67E0(unsigned char *self, int a1, float f12, float f13);
extern void func_001C47A0(int id, int on);
extern void func_001C4760(int id, int on);
extern void func_001FAE70(int a);
extern void func_001B17A0(unsigned char *self);
extern void func_001C68C0(unsigned char *self);
extern void func_001BA540(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA14_00823AF0(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    switch (self[4]) {
    case 0:
        func_001B10B0(self, self[0xD], 0x56);
        func_001C63E0(self, 0);
        func_001CA6F0(self, 2);
        func_001BA8E0(self, self[0xD]);
        self[0] = 1;
        *(char **)(self + 0x30) = D_overlay_AREA14_00827480;
        *(int *)(self + 0x58) = *(int *)0x28A5EC;
        self[4] = 1;
        break;
    case 1:
        if (func_001BA1C0(self, 0x3F) != 0) {
            self[4] = 3;
            break;
        }
        if (D_00810816[0] == 0) {
            switch (self[5]) {
            case 0:
                if (!(D_00810354[0] < 675.0f)) {
                    if (func_001B1EA0(0, D_00810350, (void *)D_overlay_AREA14_00828400, 4) != 0) {
                        func_001BA1A0(talk, D_overlay_AREA14_00826FC0);
                        self[5] = 1;
                    }
                    func_001BA580(self, self[0xD]);
                    func_001C64F0(self, 1.0f);
                }
                break;
            case 1:
                if (func_001BA1F0(self) != 0) {
                    D_00810816[0] = 1;
                    *(unsigned short *)(self + 0x2E) = 0xFFFF;
                    *(int *)(self + 0x40) = *(int *)0x28A5E8;
                    func_001C67E0(self, 0, 0.0f, 0.0f);
                    self[5] = 0;
                    func_001C47A0(0xF, 1);
                    func_001C4760(0x17, 1);
                    func_001FAE70(0);
                }
                func_001BA580(self, self[0xD]);
                *(short *)(talk + 0xE) = func_001C64F0(self, 0.5f);
                break;
            }
        } else {
            switch (self[5]) {
            case 0:
                if (self[0xB] & 4) {
                    func_001BA1A0(talk, D_overlay_AREA14_00827340);
                    self[5] = 1;
                }
                break;
            case 1:
                if (func_001BA1F0(self) != 0) {
                    func_001C67E0(self, 0, 20.0f, 0.0f);
                    self[0xB] = 0;
                    self[5] = 0;
                }
                break;
            }
            func_001BA580(self, self[0xD]);
            func_001C64F0(self, 1.0f);
        }
        func_001B17A0(self);
        self[1] = 1;
        func_001C68C0(self);
        (*(ActorFn *)(self + 0x4C))(self);
        break;
    case 2:
    case 3:
        func_001BA540(self);
        func_001AFC10(self);
        break;
    }
}
