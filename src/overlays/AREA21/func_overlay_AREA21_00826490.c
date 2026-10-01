// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA21 overlay, runtime 0x008264D0 (splat/link name 00826490; overlay code
//  is linked 0x40 below where it runs), 0x218 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA21; lane A03C).
// Covers the splat pieces 00826490, 008264D0 (the later piece is absorbed at
//  link time, tools/overlay/fill_overlay.py).
// Role: called from 0x8262D0 (+3 != 0). State 0: once func_001BA1C0(self,
//  0x34) is set: func_001B10B0(self, +0xD, 0x52), func_001C63E0(self, 0xA),
//  func_001CA6F0(self, 2), func_001BA8E0(self, +0xD), +0x30 = 0x82BA60, state
//  1, +0 = 1. State 1: +5 0: +5 2 once D_0081078D != 0; else +0xB bit 2
//  starts script 0x82B8A0 (+5 1); +5 1: at the script end
//  func_001C67E0(self, 0xA, 20.0, 0.0), +0xB = +5 = 0. func_001BA580(self,
//  +0xD) and func_001C64F0(self, 1.0) (only the latter in +5 2), then
//  func_001B17A0, func_001C68C0 and the +0x4C method. States 2 / 3:
//  func_001BA540 and func_001AFC10.
typedef void (*ActorFn)(unsigned char *);
extern unsigned char D_0081078D[];
extern char D_overlay_AREA21_0082BA60[];
extern char D_overlay_AREA21_0082B8A0[];
extern int func_001BA1C0(unsigned char *self, int idx);
extern void func_001B10B0(unsigned char *self, int a, int b);
extern void func_001C63E0(unsigned char *self, int a);
extern void func_001CA6F0(unsigned char *self, int a);
extern void func_001BA8E0(unsigned char *self, int a);
extern void func_001BA1A0(unsigned char *talk, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_001BA580(unsigned char *self, int a1);
extern short func_001C64F0(unsigned char *self, float step);
extern void func_001C67E0(unsigned char *self, int a1, float f12, float f13);
extern void func_001B17A0(unsigned char *self);
extern void func_001C68C0(unsigned char *self);
extern void func_001BA540(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA21_00826490(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    switch (self[4]) {
    case 0:
        if (func_001BA1C0(self, 0x34) != 0) {
            func_001B10B0(self, self[0xD], 0x52);
            func_001C63E0(self, 0xA);
            func_001CA6F0(self, 2);
            func_001BA8E0(self, self[0xD]);
            *(char **)(self + 0x30) = D_overlay_AREA21_0082BA60;
            self[4] = 1;
            self[0] = 1;
        }
        break;
    case 1:
        switch (self[5]) {
        case 0:
            if (D_0081078D[0] != 0) {
                self[5] = 2;
                break;
            }
            if (self[0xB] & 4) {
                func_001BA1A0(talk, D_overlay_AREA21_0082B8A0);
                self[5] = 1;
            }
            func_001BA580(self, self[0xD]);
            func_001C64F0(self, 1.0f);
            func_001B17A0(self);
            func_001C68C0(self);
            (*(ActorFn *)(self + 0x4C))(self);
            break;
        case 1:
            if (func_001BA1F0(self) != 0) {
                int zi = 0;
                float z = (float)zi;
                func_001C67E0(self, 0xA, 20.0f, z);
                self[0xB] = 0;
                self[5] = 0;
            }
            func_001BA580(self, self[0xD]);
            func_001C64F0(self, 1.0f);
            func_001B17A0(self);
            func_001C68C0(self);
            (*(ActorFn *)(self + 0x4C))(self);
            break;
        case 2:
            func_001C64F0(self, 1.0f);
            func_001B17A0(self);
            func_001C68C0(self);
            (*(ActorFn *)(self + 0x4C))(self);
            break;
        }
        break;
    case 2:
    case 3:
        func_001BA540(self);
        func_001AFC10(self);
        break;
    }
}
