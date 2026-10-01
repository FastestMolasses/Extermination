// NEARMISS func_overlay_AREA11_00823FB0 (99.99%, mwcc 2.3.3; linked from its splat .s)
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// AREA11 overlay, runtime 0x00823FF0 (splat/link name 00823FB0; overlay code is
// linked 0x40 below where it runs), 0x11F0 bytes.
// Role (the port calls it the truck set piece): state 0 (after
// func_001B0FD0): with D_00810792 == 0xFF it writes a fixed matrix at +0xD0
// (rows (0, 0, -1), (0.03927, 0.999, 0), (0.999, -0.03927, 0), position
// (371.14, 130.69, 391.158)), state 2, and copies it to the record
// *D_00275B40 +0x90; otherwise func_001C6380, +0x2E8 = +0xC0, +0x2E4 =
// +0xB4, state 4, +0x2EC = 0 and a copy of +0xD0 to +0x1F0. Both then call
// func_001A2370. State 4, counter +0x2EC 0: when D_008104C4 is set,
// D_008102BA is nonzero and that object's +0xD is 9, it calls
// func_001B1E20(0, 0), sets the counter to 1 and offsets +0x104 by
// (count % 20 - 10) / 50 and +0x100 / +0x108 by half of that (through
// 0x70003A20). Counter 1..46: a jump table on count & 15 turns +0xD0
// relative to the saved +0x1F0 matrix (func_00102B08 by -0.002 at 0,
// 0.0005 at 4, 0.0015 at 5 and 7, 0.0025 at 6; a plain copy at 3) and
// spawns effect 0x80000049 at two fixed points at 4 and at 6; at 0 it also
// sets +0xB4 = +0x2E4 - 0.04 * (count - 1); then +0x104 = +0xB4. Counter
// 47: restores +0xD0 from +0x1F0, +0xB4 = +0x2E4 - 1.8, +0x2DC = +0x2E0 = 0,
// state 1. State 1 (frame counter +0x28): the frame ranges 0-9, 15-29,
// 42-51 and 65-89 use the step (-1/30, -1/30, 0) in 0x700038A0, the ranges
// 10-14, 30-41, 52-64 and 90-118 the step (-2/15, -2/3, 0), each with a
// roll of +0xD0 (func_00102B08 by 0 then func_00102A60 by -0.003); effect
// 0x80000049 bursts at frames 8, 28, 40, 50, 64, 88 and 110 (at x/z points,
// y = +0xB4 + 20), sound 0x454 at frame 8 and 0x455 at 110, and
// func_001B1E20(2, 0) at frames 14 and 87 under the state-4 condition.
// From frame 119: D_00810792 = 0xFF, state 2, zero step. The step is added
// to +0xB0..+0xB8, and its z part (always 0 here) to the player's z
// (D_00810358, with 0x700031F0 = 1) when the player's +0xB0 x/z (D_00810360 /
// D_00810368) is inside the box that the table at *0x70003250 gives for
// (+0xE >> 8) & 0xFF. Every
// 4th frame +0x2E0 = 0.5, at frame 4 mod 16 +0x2DC = 0.2; +0x100..+0x108 =
// +0xB0 + +0x2E0, +0xB4, +0xB8 + +0x2DC. States 1 and 4 copy +0xD0 to
// *D_00275B40 +0x90 and call func_001B1B70 and the +0x4C method; state 2
// only the latter two; 3 and other values func_001AFC10.
// Divergence: one pair of argument moves. In the count & 15 == 0 case of the
//  state-4 jump table the original sets a1 (self + 0x1F0) before the call and
//  a0 (self + 0xD0) in the call's delay slot; mwcc 2.3.3 sets a0 first and
//  puts a1 in the slot. Everything else, the jump table included, is equal
//  (the checker cannot place the table; the link does, tools/overlay/jt_pin.py).
//  Tried: casts and locals on either argument (a cast on a1 moves it before
//  the float constant instead), a staged float, the blk local (drops the
//  re-materialisation), pointer spellings. Equivalent to the original.
typedef void (*ActorFn)(unsigned char *);
extern unsigned char **D_00275B40;
extern unsigned char D_00810792[8];
extern unsigned char D_008102BA[8];
extern unsigned char *D_008104C4[2];
extern float D_00810350[8];
extern float D_700038A0[4];
extern int D_700031F0[4];
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern void func_00102958(void *dst, void *src);
extern void func_00102948(void *dst, void *src);
extern void func_001A2370(unsigned char *self, void *mtx);
extern void func_001B1E20(int a0, int a1);
extern void func_00102B08(void *dst, void *src, float a);
extern void func_00102A60(void *dst, void *src, float a);
extern void func_001EFD20(int cls, void *pos);
extern void func_001FBD50(unsigned char *self, int id, int a2, float f12);
extern void func_001B1B70(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

#define F(o) (*(float *)(self + (o)))
#define I(o) (*(int *)(self + (o)))
#define S16(o) (*(short *)(self + (o)))
#define SP3A20 (*(float *)0x70003A20)
#define FX(x, y, z) { D_700038A0[0] = x; D_700038A0[1] = y; D_700038A0[2] = z; \
    D_700038A0[3] = 1.0f; func_001EFD20(0x80000049, D_700038A0); }
#define FXB(x, z) FX(x, F(0xB4) + 20.0f, z)
#define ROLL() { func_00102B08(self + 0xD0, self + 0xD0, 0.0f); \
    func_00102A60(self + 0xD0, self + 0xD0, -0.003f); }
#define STEP_A() { D_700038A0[0] = -0.0333333351f; D_700038A0[1] = -0.0333333351f; \
    D_700038A0[2] = 0.0f; }
#define STEP_B() { D_700038A0[0] = -0.13333334f; D_700038A0[1] = -0.666666687f; \
    D_700038A0[2] = 0.0f; }

void func_overlay_AREA11_00823FB0(unsigned char *self) {
    unsigned char *blk;
    int *cnt;
    unsigned char *tbl;
    float *box;
    float x;
    int t;
    switch (self[4]) {
    case 0:
        if (func_001B0FD0(self) != 0) {
            break;
        }
        if (D_00810792[0] == 0xFF) {
            F(0xD0) = 0.0f;
            F(0xD4) = 0.0f;
            F(0xD8) = -1.0f;
            F(0xDC) = 0.0f;
            F(0xE0) = 0.0392699987f;
            F(0xE4) = 0.999000013f;
            F(0xE8) = 0.0f;
            F(0xEC) = 0.0f;
            F(0xF0) = 0.999000013f;
            F(0xF4) = -0.0392699987f;
            F(0xF8) = 0.0f;
            F(0xFC) = 0.0f;
            F(0x100) = 371.140015f;
            F(0x104) = 130.690002f;
            F(0x108) = 391.15799f;
            F(0x10C) = 1.0f;
            self[4] = 2;
            func_00102958(*D_00275B40 + 0x90, self + 0xD0);
        } else {
            func_001C6380(self);
            F(0x2E8) = F(0xC0);
            F(0x2E4) = F(0xB4);
            self[4] = 4;
            I(0x2EC) = 0;
            func_00102958(self + 0x1F0, self + 0xD0);
        }
        func_001A2370(self, self + 0xD0);
        break;
    case 4:
        blk = self + 0x1F0;
        cnt = (int *)(blk + 0xFC);
        if (I(0x2EC) <= 0) {
            if (D_008104C4[0] != 0 && D_008102BA[0] != 0 && D_008104C4[0][0xD] == 9) {
                func_001B1E20(0, 0);
                (*cnt)++;
                SP3A20 = (float)(*cnt % 20 - 10) / 50.0f;
                F(0x104) = F(0xB4) + SP3A20;
                SP3A20 = SP3A20 / 2.0f;
                F(0x100) = F(0xB0) + SP3A20;
                F(0x108) = F(0xB8) - SP3A20;
                func_001A2370(self, self + 0xD0);
            }
        } else {
            (*cnt)++;
            if (*cnt > 0x2E) {
                self[4] = 1;
                func_00102958(self + 0xD0, blk);
                F(0xB4) = F(0x2E4) - 1.79999995f;
                I(0x2DC) = 0;
                I(0x2E0) = 0;
            } else {
                switch (*cnt & 0xF) {
                case 0:
                    func_00102B08(self + 0xD0, self + 0x1F0, -0.00200000009f);
                    F(0xB4) = F(0x2E4) - 0.0399999991f * (float)(*cnt - 1);
                    break;
                case 1:
                case 2:
                    break;
                case 3:
                    func_00102958(self + 0xD0, blk);
                    break;
                case 4:
                    func_00102B08(self + 0xD0, blk, 0.000500000024f);
                    func_00102948(D_700038A0, self + 0xB0);
                    FX(398.700012f, 174.800003f, 372.600006f)
                    FX(363.700012f, 162.800003f, 375.600006f)
                    break;
                case 5:
                    func_00102B08(self + 0xD0, blk, 0.00150000001f);
                    break;
                case 6:
                    func_00102B08(self + 0xD0, blk, 0.00249999994f);
                    FX(395.700012f, 163.800003f, 392.600006f)
                    FX(339.700012f, 175.800003f, 392.600006f)
                    break;
                case 7:
                    func_00102B08(self + 0xD0, blk, 0.00150000001f);
                    break;
                }
                F(0x104) = F(0xB4);
                func_001A2370(self, self + 0xD0);
            }
        }
        func_00102958(*D_00275B40 + 0x90, self + 0xD0);
        func_001B1B70(self);
        (*(ActorFn *)(self + 0x4C))(self);
        break;
    case 1:
        t = S16(0x28);
        if (t < 10) {
            if (t == 8) {
                func_001FBD50(self, 0x454, 0, 300.0f);
                FXB(398.0f, 372.600006f)
                FXB(395.700012f, 392.600006f)
            }
            ROLL()
            STEP_A()
        } else if (t < 15) {
            if (D_008104C4[0] != 0 && D_008102BA[0] != 0 && t == 14 && D_008104C4[0][0xD] == 9) {
                func_001B1E20(2, 0);
            }
            ROLL()
            STEP_B()
        } else if (t < 30) {
            if (t == 28) {
                FXB(398.0f, 372.600006f)
                FXB(395.0f, 392.600006f)
                FXB(339.700012f, 392.600006f)
                FXB(363.700012f, 375.600006f)
            }
            ROLL()
            STEP_A()
        } else if (t < 42) {
            if (t == 40) {
                FXB(339.700012f, 392.600006f)
                FXB(363.700012f, 375.600006f)
            }
            ROLL()
            STEP_B()
        } else if (t < 52) {
            if (t == 50) {
                FXB(339.700012f, 392.600006f)
                FXB(395.700012f, 392.600006f)
            }
            ROLL()
            STEP_A()
        } else if (t < 65) {
            if (t == 64) {
                FXB(339.700012f, 392.600006f)
                FXB(395.700012f, 392.600006f)
            }
            ROLL()
            STEP_B()
        } else if (t < 90) {
            if (t == 88) {
                FXB(391.0f, 372.600006f)
                FXB(395.700012f, 392.600006f)
                FXB(339.700012f, 392.600006f)
                FXB(363.700012f, 375.600006f)
            }
            if (D_008104C4[0] != 0 && D_008102BA[0] != 0 && S16(0x28) == 87 && D_008104C4[0][0xD] == 9) {
                func_001B1E20(2, 0);
            }
            ROLL()
            STEP_A()
        } else if (t < 119) {
            if (t == 110) {
                func_001FBD50(self, 0x455, 0, 300.0f);
                FXB(398.700012f, 372.600006f)
                FXB(395.700012f, 392.600006f)
                FXB(339.700012f, 392.600006f)
                FXB(363.700012f, 375.600006f)
            }
            ROLL()
            STEP_B()
        } else {
            D_00810792[0] = 0xFF;
            self[4] = 2;
            D_700038A0[2] = 0.0f;
            D_700038A0[1] = 0.0f;
            D_700038A0[0] = 0.0f;
        }
        tbl = *(unsigned char **)0x70003250;
        box = (float *)(tbl + *(int *)(tbl + 4 + ((*(unsigned short *)(self + 0xE) >> 8) & 0xFF) * 4));
        x = D_00810350[4];
        if (x > box[0] && x < box[3] && D_00810350[6] > box[2] && D_00810350[6] < box[5]) {
            D_00810350[2] += D_700038A0[2];
            D_700031F0[0] = 1;
        }
        F(0xB0) += D_700038A0[0];
        F(0xB4) += D_700038A0[1];
        F(0xB8) += D_700038A0[2];
        S16(0x28)++;
        if ((S16(0x28) & 3) == 0) {
            F(0x2E0) = 0.5f;
        }
        if ((S16(0x28) & 0xF) == 4) {
            F(0x2DC) = 0.2f;
        }
        x = F(0x2E0);
        F(0x100) = F(0xB0) + x;
        F(0x104) = F(0xB4);
        x = F(0x2DC);
        x = F(0xB8) + x;
        F(0x108) = x;
        func_00102958(*D_00275B40 + 0x90, self + 0xD0);
        func_001A2370(self, self + 0xD0);
        func_001B1B70(self);
        (*(ActorFn *)(self + 0x4C))(self);
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
