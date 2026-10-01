// NEARMISS func_overlay_AREA21_0082A190 (97.50%, mwcc 2.3.3; linked from its splat .s)
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA21 overlay, runtime 0x0082A1D0 (splat/link name 0082A190; overlay code
//  is linked 0x40 below where it runs), 0x190 bytes.
// Role: sub 0 placements [54], [55]. With +3 == 0x4C, +0xC4 moves by
//  +0x1F0 towards a random target in -pi/6 .. pi/6 (+0x1F4), choosing a new
//  target and direction (+-0.005) within 0.01 of the old one; otherwise
//  +0xC4 copies the +0x18 object's. Then func_001C6380, func_001A2370(self,
//  +0xD0) and the +0x4C method when func_001B17A0.
// Divergence: after +0xC4 += +0x1F0 and after the target store the
// original reloads +0xC4 from memory; mwcc 2.3.3 forwards the stored
// value (the AREA15 0x825430 wall), which also changes FPR choices.
typedef void (*ActorFn)(unsigned char *);
#define F(o) (*(float *)(self + (o)))
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern float func_0011DF78(float a);
extern int func_00122BB8(void);
extern void func_001A2370(unsigned char *self, void *m);
extern int func_001B17A0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA21_0082A190(unsigned char *self) {
    float *target;
    switch (self[4]) {
    case 0:
        func_001B0FD0(self);
        func_001C6380(self);
        F(0x1F0) = 0.0f;
        F(0x1F4) = 0.0f;
        break;
    case 1:
        if (self[3] == 0x4C) {
            target = (float *)(self + 0x1F0) + 1;
            F(0xC4) += F(0x1F0);
            if (func_0011DF78(F(0xC4) - F(0x1F4)) < 0.01f) {
                *target = -0.5235988f + 1.0471976f * (4.656613e-10f * (float)func_00122BB8());
                if (*target > F(0xC4)) {
                    F(0x1F0) = 0.005f;
                } else {
                    F(0x1F0) = -0.005f;
                }
            }
        } else {
            F(0xC4) = *(float *)(*(unsigned char **)(self + 0x18) + 0xC4);
        }
        func_001C6380(self);
        func_001A2370(self, self + 0xD0);
        if (func_001B17A0(self) != 0) {
            (*(ActorFn *)(self + 0x4C))(self);
        }
        break;
    case 3:
        func_001AFC10(self);
        break;
    }
}
