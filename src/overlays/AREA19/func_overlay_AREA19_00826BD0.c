// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA19 overlay, runtime 0x00826C10 (splat/link name 00826BD0; overlay code
//  is linked 0x40 below where it runs), 0x818 bytes.
// Role (read from the instructions): sub 1 placement [36]: a lift platform.
//  State 0 when func_001B0FD0 returns 0: rest height +0x2E8 = +0xB4; state 2
//  when D_00810779 == 0xFF, else state 4 (raised by 15 when D_008107F9 & 0xF)
//  and func_0019C6F0(0x15, 1). State 4: +5 0 counts +0x2EC down when it is
//  set (then speed -1/12 with func_001FBD50 0x8EB, or 1/12 with 0x8EA, and +5
//  = 1), or, with the player in the box 850 < x < 859.6, 850.5 < z < 855,
//  starts script 0x82D290 (state 1); +5 1 moves +0xB4 by the speed for 180
//  frames, then toggles D_008107F9's low nibble between 0 and 1 and snaps to
//  the rest height (+15). Four func_001F5940(3, ..) lights at the corners
//  (+-28.5, +8, +-16). State 1: when the script ends D_00810779 = 0xFF,
//  func_0019C6F0(0x15, 0), state 2; otherwise once the script callback
//  0x827540 sets +0x28 it counts to 70 (func_001FBD50 0x8EC, func_001B1E20(5,
//  120)) and then drops the platform to the rest height (speed += 0.2 a
//  frame), func_001B1E20(8, 20) once at the bottom; the lights while above
//  rest. States 1, 2 and 4: func_001B1B70 and the +0x4C method.
// Byte-identical (tools/overlay/overlay_match.py check AREA19; lane DMATCH,
//  was NEARMISS 99.94%): state 0's raise is written `(x = 15.0f) + *p`,
//  which puts the constant in f1 and the loaded value in f0 as in the
//  original, and the last rest-height read (+0x2E8, after the +0x4C method)
//  is a volatile read, which keeps it ahead of the +0xB4 load (idiom-22).
typedef void (*ActorFn)(unsigned char *);
extern unsigned char D_008107F9;
extern float D_700038A0[];
extern char D_overlay_AREA19_0082D290[];
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern void func_0019C6F0(int id, int on);
extern void func_001A2370(unsigned char *self, void *m);
extern void func_001FBD50(unsigned char *self, int id, int a2, float vol);
extern void func_001BA1A0(unsigned char *talk, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_001B1E20(int a, int b);
extern void func_001B1B70(unsigned char *self);
extern void func_001F5940(int kind, void *pos, int a2);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA19_00826BD0(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    int *p;
    float x;
    int f;
    int t;
    float lim;
    switch (self[4]) {
    case 0:
        if (func_001B0FD0(self) == 0) {
            p = (int *)(self + 0x1F0) + 0x3E;
            *(float *)(self + 0x2E8) = *(float *)(self + 0xB4);
            *(int *)(self + 0x2EC) = 0;
            if (*(unsigned char *)0x810779 == 0xFF) {
                func_001C6380(self);
                self[4] = 2;
            } else {
                if (D_008107F9 & 0xF) {
                    *(float *)(self + 0xB4) = (x = 15.0f) + *(float *)p;
                }
                func_001C6380(self);
                self[4] = 4;
                func_0019C6F0(0x15, 1);
            }
            func_001A2370(self, self + 0xD0);
        }
        break;
    case 4:
        switch (self[5]) {
        case 0:
            p = (int *)talk + 0x3F;
            if (*(int *)(self + 0x2EC) != 0) {
                if (*(unsigned char *)0x810779 == 0xFF) {
                    *p = 0;
                } else {
                    *p -= 1;
                    if (*p == 0) {
                        if (*(unsigned char *)0x8107F9 & 0xF) {
                            *(float *)(self + 0x2E4) = -0.083333336f;
                            func_001FBD50(self, 0x8EB, 0, 300.0f);
                        } else {
                            *(float *)(self + 0x2E4) = 0.083333336f;
                            func_001FBD50(self, 0x8EA, 0, 300.0f);
                        }
                        self[5]++;
                        *(short *)(self + 0x28) = 0;
                    }
                }
            } else {
                x = *(float *)0x810350;
                if (!(x <= 850.0f) && x < 859.6f) {
                    x = *(float *)0x810358;
                    if (!(x <= 850.5f) && x < 855.0f) {
                        *(float *)(self + 0x2E4) = 0.0f;
                        *(int *)(self + 0x2E0) = 0;
                        *(short *)(self + 0x28) = 0;
                        func_001BA1A0(talk, D_overlay_AREA19_0082D290);
                        self[4] = 1;
                    }
                }
            }
            break;
        case 1:
            *(short *)(self + 0x28) += 1;
            if (*(short *)(self + 0x28) < 0xB4) {
                *(float *)(self + 0xB4) += *(float *)(self + 0x2E4);
                func_001C6380(self);
                func_001A2370(self, self + 0xD0);
            } else {
                f = D_008107F9;
                *(unsigned char *)0x8107F9 = (f & 0xF0) | !(f & 0xF);
                if (*(unsigned char *)0x8107F9 & 0xF) {
                    *(float *)(self + 0xB4) = 15.0f + *(float *)(self + 0x2E8);
                } else {
                    *(float *)(self + 0xB4) = *(float *)(self + 0x2E8);
                }
                self[5] = 0;
                func_001C6380(self);
                func_001A2370(self, self + 0xD0);
            }
            break;
        }
        func_001B1B70(self);
        (*(ActorFn *)(self + 0x4C))(self);
        *(float *)0x700038AC = 1.0f;
        *(float *)0x700038A0 = 28.5f + *(float *)(self + 0xB0);
        *(float *)0x700038A4 = 8.0f + *(float *)(self + 0xB4);
        *(float *)0x700038A8 = 16.0f + *(float *)(self + 0xB8);
        func_001F5940(3, D_700038A0, 0);
        *(float *)0x700038A0 = 28.5f + *(float *)(self + 0xB0);
        *(float *)0x700038A4 = 8.0f + *(float *)(self + 0xB4);
        *(float *)0x700038A8 = *(float *)(self + 0xB8) - 16.0f;
        func_001F5940(3, D_700038A0, 0);
        *(float *)0x700038A0 = *(float *)(self + 0xB0) - 28.5f;
        *(float *)0x700038A4 = 8.0f + *(float *)(self + 0xB4);
        *(float *)0x700038A8 = *(float *)(self + 0xB8) - 16.0f;
        func_001F5940(3, D_700038A0, 0);
        *(float *)0x700038A0 = *(float *)(self + 0xB0) - 28.5f;
        *(float *)0x700038A4 = 8.0f + *(float *)(self + 0xB4);
        *(float *)0x700038A8 = 16.0f + *(float *)(self + 0xB8);
        func_001F5940(3, D_700038A0, 0);
        break;
    case 1:
        if (func_001BA1F0(self) != 0) {
            *(unsigned char *)0x810779 = 0xFF;
            func_0019C6F0(0x15, 0);
            self[4] = 2;
            break;
        }
        t = *(short *)(self + 0x28);
        if (t != 0) {
            if ((short)t < 0x46) {
                *(short *)(self + 0x28) = t + 1;
                if (*(short *)(self + 0x28) == 0x46) {
                    func_001FBD50(self, 0x8EC, 0, 300.0f);
                    func_001B1E20(5, 0x78);
                }
            } else {
                lim = *(float *)(self + 0x2E8);
                if (!(*(float *)(self + 0xB4) <= lim)) {
                    *(float *)(self + 0x2E4) += 0.2f;
                    *(float *)(self + 0xB4) = *(float *)(self + 0xB4) - *(float *)(self + 0x2E4);
                    func_001C6380(self);
                    func_001A2370(self, self + 0xD0);
                } else {
                    *(float *)(self + 0xB4) = lim;
                    p = (int *)(self + 0x1F0) + 0x3C;
                    if (*(int *)(self + 0x2E0) == 0) {
                        func_001B1E20(8, 0x14);
                        *p = 1;
                    }
                }
            }
        }
        func_001B1B70(self);
        (*(ActorFn *)(self + 0x4C))(self);
        lim = *(volatile float *)(self + 0x2E8);
        if (!(*(float *)(self + 0xB4) <= lim)) {
            *(float *)0x700038AC = 1.0f;
            *(float *)0x700038A0 = 28.5f + *(float *)(self + 0xB0);
            *(float *)0x700038A4 = 8.0f + *(float *)(self + 0xB4);
            *(float *)0x700038A8 = 16.0f + *(float *)(self + 0xB8);
            func_001F5940(3, D_700038A0, 0);
            *(float *)0x700038A0 = 28.5f + *(float *)(self + 0xB0);
            *(float *)0x700038A4 = 8.0f + *(float *)(self + 0xB4);
            *(float *)0x700038A8 = *(float *)(self + 0xB8) - 16.0f;
            func_001F5940(3, D_700038A0, 0);
            *(float *)0x700038A0 = *(float *)(self + 0xB0) - 28.5f;
            *(float *)0x700038A4 = 8.0f + *(float *)(self + 0xB4);
            *(float *)0x700038A8 = *(float *)(self + 0xB8) - 16.0f;
            func_001F5940(3, D_700038A0, 0);
            *(float *)0x700038A0 = *(float *)(self + 0xB0) - 28.5f;
            *(float *)0x700038A4 = 8.0f + *(float *)(self + 0xB4);
            *(float *)0x700038A8 = 16.0f + *(float *)(self + 0xB8);
            func_001F5940(3, D_700038A0, 0);
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
