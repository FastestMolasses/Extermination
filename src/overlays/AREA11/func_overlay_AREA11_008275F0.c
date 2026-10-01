// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA11 overlay, runtime 0x00827630 (splat/link name 008275F0; overlay code is
// linked 0x40 below where it runs), 0x4DC bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA11; lane A11C).
// Role: fan pair (+0x2E selects the unit, 0 or 1). State 0:
// func_001B0FD0, speed +0x38 = 0, angle +0xC8 = +-pi/4. State 1 by +5: 0
// +0x28 = 60; 1 counts down (sound 0x451 for unit 0 unless D_00810788 ==
// 1); 2 spins up by 0.00290888 per frame to 0.34906587, then +0x28 = 30; 3
// counts down and sets the angle to -+1.8325958; 4 spins down to 0 and
// back to +5 0. The angle turns by +-speed (func_001B1470 wraps it),
// func_001C6380. For unit 1 with D_008106B8 == 0: at speed >= 0.0349 it
// calls func_001B17A0 and, for the player record D_008102B0 in state 1
// inside y 280..320 and x 318..340, z < 156 calls func_001B0C60(1, 1, 4)
// when D_00810758 == 0xFF (else D_008107D8 |= 0x80), and z < 166.5 blows
// the player (+0x224 = 5, +0 = 3, +0xF = 6, +0x70 = +0x74 = 0, +0x78 =
// +0x7C = 1.0); below that speed only the z < 156 case applies (without
// the state check). Then the +0x4C method. States 2/3: func_001AFC10.
typedef void (*ActorFn)(unsigned char *);
extern unsigned char D_00810788;
extern unsigned char D_008106B8;
extern unsigned char D_00810758;
extern unsigned char D_008107D8;
extern unsigned char D_008102B0[];
extern int func_001B0FD0(unsigned char *self);
extern void func_001FBD50(unsigned char *self, int id, int a2, float f12);
extern float func_001B1470(float a);
extern void func_001C6380(unsigned char *self);
extern int func_001B17A0(unsigned char *self);
extern void func_001B0C60(int a0, int a1, int a2);
extern void func_001AFC10(unsigned char *self);

#define F(o) (*(float *)(self + (o)))
#define PF(o) (*(float *)(pl + (o)))
#define S16(o) (*(short *)(self + (o)))
#define U16(o) (*(unsigned short *)(self + (o)))

void func_overlay_AREA11_008275F0(unsigned char *self) {
    unsigned char *pl = D_008102B0;
    float x;
    switch (self[4]) {
    case 0:
        func_001B0FD0(self);
        *(int *)(self + 0x38) = 0;
        if (U16(0x2E) == 0) {
            F(0xC8) = 0.785398185f;
        } else {
            F(0xC8) = -0.785398185f;
        }
        break;
    case 1:
        switch (self[5]) {
        case 0:
            S16(0x28) = 60;
            self[5]++;
            break;
        case 1:
            if (--S16(0x28) == 0) {
                self[5]++;
                if (U16(0x2E) == 0 && D_00810788 != 1) {
                    func_001FBD50(self, 0x451, 0, 300.0f);
                }
            }
            break;
        case 2:
            F(0x38) += 0.00290888222f;
            if (!(F(0x38) < 0.34906587f)) {
                self[5]++;
                S16(0x28) = 30;
            }
            break;
        case 3:
            if (--S16(0x28) == 0) {
                self[5]++;
                if (U16(0x2E) == 0) {
                    F(0xC8) = -1.83259583f;
                } else {
                    F(0xC8) = 1.83259583f;
                }
            }
            break;
        case 4:
            x = F(0x38) - 0.00290888222f;
            F(0x38) = x;
            if (x <= 0.0f) {
                *(int *)(self + 0x38) = 0;
                self[5] = 0;
            }
            break;
        }
        if (U16(0x2E) == 0) {
            F(0xC8) += F(0x38);
        } else {
            F(0xC8) -= F(0x38);
        }
        F(0xC8) = func_001B1470(F(0xC8));
        func_001C6380(self);
        if (U16(0x2E) == 1 && D_008106B8 == 0) {
            if (!(F(0x38) < 0.0349065848f)) {
                func_001B17A0(self);
                if (pl[0] == 1 && PF(0xA4) > 280.0f && PF(0xA4) < 320.0f &&
                    PF(0xA0) > 318.0f && PF(0xA0) < 340.0f) {
                    if (PF(0xA8) < 156.0f) {
                        if (D_00810758 == 0xFF) {
                            func_001B0C60(1, 1, 4);
                        } else {
                            D_008107D8 |= 0x80;
                        }
                    } else if (PF(0xA8) < 166.5f) {
                        PF(0x224) = 5.0f;
                        pl[0] = 3;
                        pl[0xF] = 6;
                        *(int *)(pl + 0x70) = 0;
                        *(int *)(pl + 0x74) = 0;
                        PF(0x78) = 1.0f;
                        PF(0x7C) = 1.0f;
                    }
                }
            } else if (PF(0xA4) > 280.0f && PF(0xA4) < 320.0f &&
                       PF(0xA0) > 318.0f && PF(0xA0) < 340.0f && PF(0xA8) < 156.0f) {
                if (D_00810758 == 0xFF) {
                    func_001B0C60(1, 1, 4);
                } else {
                    D_008107D8 |= 0x80;
                }
            }
        }
        (*(ActorFn *)(self + 0x4C))(self);
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
