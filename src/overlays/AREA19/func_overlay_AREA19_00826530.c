// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA19 overlay, runtime 0x00826570 (splat/link name 00826530; overlay code
//  is linked 0x40 below where it runs), 0x2CC bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA19; lane A13C).
// Role: sub 1 placement [38]. State 0 when func_001B0FD0 returns 0: state 2
//  when func_001BA1C0(self, 0x20) or (0x23) is set, else func_0019C6F0(7, 1),
//  +0x30 = 0x82CC60, state 1, +0 = 1. State 1: +5 0 on +0xB bit 2:
//  func_001B6F00(self, (0, 0, 5, 1), pi), script 0x82CA20; +5 1:
//  func_001FBD50(self, 0x19A, 0, 300.0) at frame 120; when the script ends
//  func_001F6BA0(), func_0019C6F0(7, 0), D_00810778 = D_008107F8 = 0xFF,
//  func_001B6660(0x82AB00) when D_0081077B == 0xFF, +2 = 4, state 2. Then the
//  +0x4C method when func_001B17A0 is set and a func_001F4BF0 light (0, 0x80,
//  0, 0x80) at (890.4, 384.9, 836.37).
typedef void (*ActorFn)(unsigned char *);
extern float D_700038A0[];
extern int D_700038B0[];
extern char D_overlay_AREA19_0082AB00[];
extern char D_overlay_AREA19_0082CA20[];
extern char D_overlay_AREA19_0082CC60[];
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern int func_001BA1C0(unsigned char *self, int idx);
extern void func_0019C6F0(int id, int on);
extern void func_001B6F00(unsigned char *self, void *v, float a);
extern void func_001BA1A0(unsigned char *talk, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_001FBD50(unsigned char *self, int id, int a2, float vol);
extern void func_001F6BA0(void);
extern void *func_001B6660(void *p);
extern int func_001B17A0(unsigned char *self);
extern void func_001F4BF0(void *pos, void *col);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA19_00826530(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    short t;
    switch (self[4]) {
    case 0:
        if (func_001B0FD0(self) == 0) {
            func_001C6380(self);
            if (func_001BA1C0(self, 0x20) != 0 || func_001BA1C0(self, 0x23) != 0) {
                self[4] = 2;
            } else {
                func_0019C6F0(7, 1);
                *(void **)(self + 0x30) = D_overlay_AREA19_0082CC60;
                self[4] = 1;
                self[0] = 1;
            }
        }
        break;
    case 1:
        switch (self[5]) {
        case 0:
            if (self[0xB] & 4) {
                *(short *)(self + 0x28) = 0;
                *(float *)0x700038A0 = 0.0f;
                *(float *)0x700038A4 = 0.0f;
                *(float *)0x700038A8 = 5.0f;
                *(float *)0x700038AC = 1.0f;
                func_001B6F00(self, D_700038A0, 3.1415927f);
                func_001BA1A0(talk, D_overlay_AREA19_0082CA20);
                self[5] = 1;
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
                func_001F6BA0();
                func_0019C6F0(7, 0);
                *(unsigned char *)0x810778 = 0xFF;
                *(unsigned char *)0x8107F8 = 0xFF;
                if (*(unsigned char *)0x81077B == 0xFF) {
                    func_001B6660(D_overlay_AREA19_0082AB00);
                }
                self[2] = 4;
                self[4] = 2;
            }
            break;
        }
        if (func_001B17A0(self) != 0) {
            (*(ActorFn *)(self + 0x4C))(self);
        }
        D_700038B0[0] = 0;
        *(float *)0x700038A0 = 890.4f;
        *(float *)0x700038A4 = 384.9f;
        *(float *)0x700038A8 = 836.37f;
        *(float *)0x700038AC = 1.0f;
        *(int *)0x700038B4 = 0x80;
        *(int *)0x700038B8 = 0;
        *(int *)0x700038BC = 0x80;
        func_001F4BF0(D_700038A0, D_700038B0);
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
