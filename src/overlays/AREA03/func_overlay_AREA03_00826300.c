// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// AREA03 overlay, runtime 0x00826340 (splat/link name 00826300; overlay code
//  is linked 0x40 below where it runs), 0x290 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA03; lane A03C).
// Role: sub 0 placement [11]. State 0: func_001C6380, state 1, +0 = 1, +8 =
//  1, +0x30 = 0x828FF0. State 1: +0 = 2 with D_00275CA0 set, else 1. +5 0:
//  on +0xB bit 2 script 0x828BD0 (D_00275CA0 set) or 0x828CD0,
//  func_001B6F00(self, (0, 0, 5.6, 1), -2.9670596), +0x28 = 0, +5 = 1. +5 1
//  (and above): +0x28 counts to 120 (func_001FBD50(self, 0x19A, 0, 300.0)),
//  +0xB and +5 cleared at the script end. Then the +0x4C method when
//  func_001B17A0 is nonzero and a func_001F4BF0 marker at (547.5613,
//  94.9041, 411.6271), (0x80, 0, 0, 0x80) with D_00275CA0 set, else
//  (0, 0x80, 0, 0x80). States 2, 3 and
//  above: func_001AFC10.
typedef void (*ActorFn)(unsigned char *);
#define S16(o) (*(short *)(self + (o)))
extern int D_00275CA0;
extern float D_700038A0[4];
extern int D_700038B0[4];
extern char D_overlay_AREA03_00828FF0[];
extern char D_overlay_AREA03_00828BD0[];
extern char D_overlay_AREA03_00828CD0[];
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern void func_001BA1A0(unsigned char *talk, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_001B6F00(unsigned char *self, void *pos, float yaw);
extern void func_001FBD50(unsigned char *self, int id, int a2, float f12);
extern int func_001B17A0(unsigned char *self);
extern void func_001F4BF0(void *pos, void *rgba);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA03_00826300(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    switch (self[4]) {
    case 0:
        if (func_001B0FD0(self) != 0) {
            break;
        }
        func_001C6380(self);
        self[4] = 1;
        self[0] = 1;
        self[8] = 1;
        *(char **)(self + 0x30) = D_overlay_AREA03_00828FF0;
        break;
    case 1:
        if (D_00275CA0 != 0) {
            self[0] = 2;
        } else {
            self[0] = 1;
        }
        switch (self[5]) {
        case 0:
            if (self[0xB] & 4) {
                if (D_00275CA0 != 0) {
                    func_001BA1A0(talk, D_overlay_AREA03_00828BD0);
                } else {
                    func_001BA1A0(talk, D_overlay_AREA03_00828CD0);
                }
                D_700038A0[0] = 0.0f;
                D_700038A0[1] = 0.0f;
                D_700038A0[2] = 5.6f;
                D_700038A0[3] = 1.0f;
                func_001B6F00(self, D_700038A0, -2.9670596f);
                S16(0x28) = 0;
                self[5] = 1;
            }
            break;
        case 1:
        default:
            if (S16(0x28) < 0x78) {
                S16(0x28)++;
                if (S16(0x28) == 0x78) {
                    func_001FBD50(self, 0x19A, 0, 300.0f);
                }
            }
            if (func_001BA1F0(self) != 0) {
                self[0xB] = 0;
                self[5] = 0;
            }
            break;
        }
        if (func_001B17A0(self) != 0) {
            (*(ActorFn *)(self + 0x4C))(self);
        }
        D_700038A0[0] = 547.5613f;
        D_700038A0[1] = 94.9041f;
        D_700038A0[2] = 411.6271f;
        D_700038A0[3] = 1.0f;
        if (D_00275CA0 != 0) {
            D_700038B0[0] = 0x80;
            D_700038B0[1] = 0;
            D_700038B0[2] = 0;
            D_700038B0[3] = 0x80;
        } else {
            D_700038B0[0] = 0;
            D_700038B0[1] = 0x80;
            D_700038B0[2] = 0;
            D_700038B0[3] = 0x80;
        }
        func_001F4BF0(D_700038A0, D_700038B0);
        break;
    case 3:
    default:
        func_001AFC10(self);
        break;
    }
}
