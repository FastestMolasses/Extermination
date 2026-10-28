// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// Two-step actor sub-state handler on the step byte self+7.
// Step 0: func_001B5360(self), then eases the angle self+0xC0 toward pi/2 by
// 4 degrees (0.0698 rad) per tick through func_001B12B0(pi/2, angle, step).
// When the controller's anim id ctl+0xE4 is 0x100 it starts clip 9
// (func_001287F0(self, ctl, 9, 0.0)), zeroes the angle, advances the step and
// clears ctl+0xD8 and the halfword ctl+0xF4.
// Step 1: waits for bit 0x1000 of ctl+0xF4 (clip finished); then
// func_00128830(self, 0, 0, 1.0), clips 0 (speed 0) and 2 (speed 6.0), sets
// state self+5 = 1, clears the sub-state bytes self+6/self+7 and copies the
// home position self+0xB0 into ctl+0x50 (func_00102948).

extern void func_001B5360(char *p);
extern float func_001B12B0(float cur, float target, float step);
extern void func_001287F0(char *a, char *b, int c, float f);
extern void func_00128830(char *arg0, float fparg0, float fparg1, float fparg2);
extern void func_00102948(void *dst, void *src);

void func_0012DD70(char *self, char *ctl) {
    switch (*(unsigned char *)(self + 7)) {
    case 0:
        func_001B5360(self);
        *(float *)(self + 0xC0) = func_001B12B0(1.5707964f, *(float *)(self + 0xC0), 0.06981317f);
        if (*(int *)(ctl + 0xE4) == 0x100) {
            func_001287F0(self, ctl, 9, 0.0f);
            *(float *)(self + 0xC0) = 0.0f;
            *(unsigned char *)(self + 7) += 1;
            *(int *)(ctl + 0xD8) = 0;
            *(short *)(ctl + 0xF4) = 0;
        }
        break;
    case 1:
        if (*(short *)(ctl + 0xF4) & 0x1000) {
            func_00128830(self, 0.0f, 0.0f, 1.0f);
            func_001287F0(self, ctl, 0, 0.0f);
            func_001287F0(self, ctl, 2, 6.0f);
            *(unsigned char *)(self + 5) = 1;
            *(unsigned char *)(self + 6) = 0;
            *(unsigned char *)(self + 7) = 0;
            func_00102948(ctl + 0x50, self + 0xB0);
        }
        break;
    }
}
