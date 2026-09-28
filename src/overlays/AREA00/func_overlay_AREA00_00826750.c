// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA00 overlay, runtime 0x00826790 (splat/link name 00826750; overlay
// code is linked 0x40 below where it runs). Byte-identical (objdiff 100%,
// tools/overlay/overlay_match.py check AREA00).
// Role: state 0 inits (+0x30 = 0x82CD00; func_001F6AD0 and state 3 when
//  D_00810784 is 0xFF) and stores two transformed points at +0x2DC..+0x2E8.
//  State 1: +5 0 on +0xB bit 2 either (bit 0) sets D_00810804 = 2,
//  func_001C47E0(0x26, 1), +0x2EC = 1 and script 0x82D070, or starts script
//  D_00246F20 (event 0x8000001C); +5 1 at script end with +0x2EC set calls
//  func_001B0C60(0xE, 0, 0) and state 2; with +0x2EC == 2 counts +0x28 and at
//  824 runs func_001F6AD0 and func_001B6660(0x827FB0) with D_00810784 = 0xFF.
typedef void (*ActorFn)(unsigned char *);
extern unsigned char D_00810784;
extern unsigned char D_00810804;
extern unsigned char D_70003B92;
extern int D_00246FB4;
extern char D_00246F20[];
extern float D_700038A0[4];
extern float D_700038B0[4];
extern char D_overlay_AREA00_0082CD00[];
extern char D_overlay_AREA00_0082D070[];
extern char D_overlay_AREA00_00827FB0[];
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern void func_001F6AD0(void);
extern void func_001026A0(void *a, void *b, void *c);
extern void func_001C47E0(int id, int a);
extern void func_001BA1A0(unsigned char *, unsigned char *);
extern int func_001BA1F0(unsigned char *self);
extern void func_001F5940(int a0, float *a1, int a2);
extern void func_001B0C60(int a0, int a1, int a2);
extern unsigned char *func_001B6660(void *p);
extern void func_001B1B70(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA00_00826750(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    int *st;
    int b;
    switch (self[4]) {
    case 0:
        if (func_001B0FD0(self) == 0) {
            func_001C6380(self);
            self[0] = 1;
            self[8] = 1;
            *(char **)(self + 0x30) = D_overlay_AREA00_0082CD00;
            if (D_00810784 == 0xFF) {
                func_001F6AD0();
                self[4] = 3;
            }
            D_700038A0[3] = 1.0f;
            D_700038A0[0] = -11.017f;
            D_700038A0[1] = 26.99f;
            D_700038A0[2] = -41.0f;
            func_001026A0(D_700038B0, self + 0xD0, D_700038A0);
            *(float *)(self + 0x2E8) = D_700038B0[0];
            *(float *)(self + 0x2E4) = D_700038B0[1];
            *(float *)(self + 0x2E0) = D_700038B0[2];
            D_700038A0[2] = -36.954f;
            func_001026A0(D_700038B0, self + 0xD0, D_700038A0);
            *(float *)(self + 0x2DC) = D_700038B0[0];
            *(float *)(self + 0x2D8) = D_700038B0[1];
            *(float *)(self + 0x2D4) = D_700038B0[2];
        }
        break;
    case 1:
        switch (self[5]) {
        case 0:
            b = self[0xB];
            if (b & 4) {
                if (b & 1) {
                    D_00810804 = 2;
                    func_001C47E0(0x26, 1);
                    *(int *)(self + 0x2EC) = 1;
                    self[5]++;
                    *(short *)(self + 0x28) = 0;
                    func_001BA1A0(talk, (unsigned char *)D_overlay_AREA00_0082D070);
                } else {
                    D_00246FB4 = 0x8000001C;
                    func_001BA1A0(talk, (unsigned char *)D_00246F20);
                    *(int *)(self + 0x2EC) = 0;
                    self[5]++;
                }
            }
            D_700038A0[3] = 1.0f;
            D_700038A0[0] = *(float *)(self + 0x2E8);
            D_700038A0[1] = *(float *)(self + 0x2E4);
            D_700038A0[2] = *(float *)(self + 0x2E0);
            func_001F5940(9, D_700038A0, 0);
            D_700038A0[0] = *(float *)(self + 0x2DC);
            D_700038A0[1] = *(float *)(self + 0x2D8);
            D_700038A0[2] = *(float *)(self + 0x2D4);
            func_001F5940(9, D_700038A0, 0);
            break;
        case 1:
            if (func_001BA1F0(self) != 0) {
                if (*(int *)(self + 0x2EC) != 0) {
                    *(unsigned short *)(self + 0x2E) = 0xFFFF;
                    func_001B0C60(0xE, 0, 0);
                    self[4] = 2;
                } else {
                    self[5] = 0;
                    self[0xB] = 0;
                }
            }
            st = (int *)(self + 0x1F0) + 0x3F;
            if (*st == 2 && *(short *)(self + 0x28) < 1000) {
                (*(short *)(self + 0x28))++;
                if (*(short *)(self + 0x28) == 824) {
                    unsigned char saved = D_00810784;
                    D_00810784 = 0xFF;
                    func_001F6AD0();
                    func_001B6660(D_overlay_AREA00_00827FB0);
                    D_00810784 = saved;
                }
            }
            if (*st == 0) {
                D_700038A0[3] = 1.0f;
                D_700038A0[0] = *(float *)(self + 0x2E8);
                D_700038A0[1] = *(float *)(self + 0x2E4);
                D_700038A0[2] = *(float *)(self + 0x2E0);
                func_001F5940(9, D_700038A0, 0);
                D_700038A0[0] = *(float *)(self + 0x2DC);
                D_700038A0[1] = *(float *)(self + 0x2D8);
                D_700038A0[2] = *(float *)(self + 0x2D4);
                func_001F5940(9, D_700038A0, 0);
            }
            break;
        }
        if (D_70003B92 == 0 || *(int *)(self + 0x2EC) == 0) {
            func_001B1B70(self);
            (*(ActorFn *)(self + 0x4C))(self);
        }
        break;
    case 2:
        func_001B1B70(self);
        (*(ActorFn *)(self + 0x4C))(self);
        break;
    case 3:
    default:
        func_001AFC10(self);
        break;
    }
}
