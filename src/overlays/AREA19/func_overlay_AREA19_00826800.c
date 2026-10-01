// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA19 overlay, runtime 0x00826840 (splat/link name 00826800; overlay code
//  is linked 0x40 below where it runs), 0x2EC bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA19; lane A13C).
// Role: sub 1 placement [37]. State 0 when func_001B0FD0 returns 0: +0 = 1;
//  state 2 (+2 = 4) when D_00810779 == 0xFF, else +8 = 1, +0x30 = 0x82D270,
//  state 1. State 1: state 2 once D_00810779 == 0xFF; +5 0 on +0xB bit 2:
//  script 0x82CFF0 when D_008107F9 & 0xF, else script 0x82CC70 and D_008107F9
//  |= 0x80; func_001B6F00(self, (0, 0, 6.2, 1), pi); the +0x18 object's
//  +0x2EC = 0x8A. +5 1: func_001FBD50(self, 0x19A, 0, 300.0) at frame 120;
//  when the script ends +5 = +0xB = 0 and D_008107F9 &= 0x7F. Then
//  func_001B17A0, the +0x4C method and a func_001F4BF0 light (0, 0x80, 0,
//  0x80) at (819.4, 384.9, 836.35).
typedef void (*ActorFn)(unsigned char *);
extern unsigned char D_00810779;
extern float D_700038A0[];
extern int D_700038B0[];
extern char D_overlay_AREA19_0082CC70[];
extern char D_overlay_AREA19_0082CFF0[];
extern char D_overlay_AREA19_0082D270[];
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern void func_001BA1A0(unsigned char *talk, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_001B6F00(unsigned char *self, void *v, float a);
extern void func_001FBD50(unsigned char *self, int id, int a2, float vol);
extern void func_001B17A0(unsigned char *self);
extern void func_001F4BF0(void *pos, void *col);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA19_00826800(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    short t;
    switch (self[4]) {
    case 0:
        if (func_001B0FD0(self) == 0) {
            func_001C6380(self);
            self[0] = 1;
            if (*(unsigned char *)0x810779 == 0xFF) {
                self[2] = 4;
                self[4] = 2;
            } else {
                self[8] = 1;
                *(void **)(self + 0x30) = D_overlay_AREA19_0082D270;
                self[4] = 1;
            }
        }
        break;
    case 1:
        if (D_00810779 == 0xFF) {
            self[2] = 4;
            self[4] = 2;
            break;
        }
        switch (self[5]) {
        case 0:
            if (self[0xB] & 4) {
                *(short *)(self + 0x28) = 0;
                if (*(unsigned char *)0x8107F9 & 0xF) {
                    func_001BA1A0(talk, D_overlay_AREA19_0082CFF0);
                } else {
                    func_001BA1A0(talk, D_overlay_AREA19_0082CC70);
                    *(unsigned char *)0x8107F9 |= 0x80;
                }
                D_700038A0[0] = 0.0f;
                *(float *)0x700038A4 = 0.0f;
                *(float *)0x700038A8 = 6.2f;
                *(float *)0x700038AC = 1.0f;
                func_001B6F00(self, D_700038A0, 3.1415927f);
                *(int *)(*(unsigned char **)(self + 0x18) + 0x2EC) = 0x8A;
                self[5]++;
            }
            break;
        case 1:
            t = *(short *)(self + 0x28);
            if (t < 0x78) {
                *(short *)(self + 0x28) = t + 1;
                if (*(short *)(self + 0x28) == 0x78) {
                    func_001FBD50(self, 0x19A, 0, 300.0f);
                }
            }
            if (func_001BA1F0(self) != 0) {
                self[5] = 0;
                self[0xB] = 0;
                *(unsigned char *)0x8107F9 &= 0x7F;
            }
            break;
        }
        func_001B17A0(self);
        (*(ActorFn *)(self + 0x4C))(self);
        D_700038B0[0] = 0;
        *(float *)0x700038A0 = 819.4f;
        *(float *)0x700038A4 = 384.9f;
        *(float *)0x700038A8 = 836.35f;
        *(float *)0x700038AC = 1.0f;
        *(int *)0x700038B4 = 0x80;
        *(int *)0x700038B8 = 0;
        *(int *)0x700038BC = 0x80;
        func_001F4BF0(D_700038A0, D_700038B0);
        break;
    case 2:
        func_001B17A0(self);
        (*(ActorFn *)(self + 0x4C))(self);
        break;
    case 3:
    default:
        func_001AFC10(self);
        break;
    }
}
