// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA08 overlay, runtime 0x00824210 (splat/link name 008241D0; overlay code
//  is linked 0x40 below where it runs), 0x244 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA08; lane OVLC).
// Role: sub 0 / 1 placement [12] (counter 0x17). State 0: state 3 when
//  func_001BA1C0(self, 0x17); else func_001B10B0(self, +0xD, 0x62),
//  func_001C63E0(self, 0), func_001CA6F0(self, 2), func_001BA8E0(self, +0xD),
//  state 1, +0 = 1. State 1 (only while D_0081076F is set): D_008107EF 0: +5
//  0 script 0x825AA0; +5 1 at its end +0x2E = 0xFFFF, D_008107EF = 1,
//  D_008106C0 = func_001B6660(group 0x825420), func_001FAE70(0). D_008107EF
//  set: +5 0 at spawn entry 6 script 0x825E60 (+0x28 = 0); +5 1
//  func_001BA580, at the script end func_001C4760(0x63, 1), D_008107EF =
//  0xFF, state 3; and 0x824490 each frame. Then func_001C64F0(self, 1.0),
//  func_001B17A0, func_001C68C0 and the +0x4C method. States 2 / 3
//  func_001BA540 and func_001AFC10.
typedef void (*ActorFn)(unsigned char *);
extern unsigned char D_0081076F;
extern unsigned char D_008107EF;
extern unsigned char D_00810702;
extern void *D_008106C0;
extern char D_overlay_AREA08_00825AA0[];
extern char D_overlay_AREA08_00825E60[];
extern char D_overlay_AREA08_00825420[];
extern int func_001BA1C0(unsigned char *self, int idx);
extern void func_001B10B0(unsigned char *self, int a, int b);
extern void func_001C63E0(unsigned char *self, int a);
extern void func_001CA6F0(unsigned char *self, int a);
extern void func_001BA8E0(unsigned char *self, int a);
extern void func_001BA1A0(unsigned char *talk, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_001BA580(unsigned char *self, int a1);
extern void *func_001B6660(void *grp);
extern void func_001FAE70(int a);
extern void func_001C4760(int id, int n);
extern void func_overlay_AREA08_00824490(unsigned char *self);
extern short func_001C64F0(unsigned char *self, float step);
extern void func_001B17A0(unsigned char *self);
extern void func_001C68C0(unsigned char *self);
extern void func_001BA540(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA08_008241D0(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    switch (self[4]) {
    case 0:
        if (func_001BA1C0(self, 0x17) != 0) {
            self[4] = 3;
            break;
        }
        func_001B10B0(self, self[0xD], 0x62);
        func_001C63E0(self, 0);
        func_001CA6F0(self, 2);
        func_001BA8E0(self, self[0xD]);
        self[4] = 1;
        self[0] = 1;
        break;
    case 1:
        if (D_0081076F == 0) {
            break;
        }
        switch (D_008107EF) {
        case 0:
            switch (self[5]) {
            case 0:
                func_001BA1A0(talk, D_overlay_AREA08_00825AA0);
                self[5] = 1;
                break;
            case 1:
                if (func_001BA1F0(self) != 0) {
                    *(unsigned short *)(self + 0x2E) = 0xFFFF;
                    self[5] = 0;
                    D_008107EF = 1;
                    D_008106C0 = func_001B6660(D_overlay_AREA08_00825420);
                    func_001FAE70(0);
                }
                break;
            }
            break;
        default:
            switch (self[5]) {
            case 0:
                if (D_00810702 == 6) {
                    func_001BA1A0(talk, D_overlay_AREA08_00825E60);
                    self[5] = 1;
                    *(short *)(self + 0x28) = 0;
                }
                break;
            case 1:
                func_001BA580(self, self[0xD]);
                if (func_001BA1F0(self) != 0) {
                    func_001C4760(0x63, 1);
                    D_008107EF = 0xFF;
                    self[4] = 3;
                }
                func_overlay_AREA08_00824490(self);
                break;
            }
            break;
        }
        func_001C64F0(self, 1.0f);
        func_001B17A0(self);
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
