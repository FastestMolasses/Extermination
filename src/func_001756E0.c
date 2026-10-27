// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 99.74% NEARMISS).
// Fixes read from the original instructions:
// - The saved old latch value is an int, not an unsigned char (the byte is
//   still read zero-extended); its type changes its register priority.
// - Locals are declared loop counter, old latch, yaw pointer: that order
//   gives the counter, old latch and table pointer the original saved
//   registers.
//
// Low-clearance latch update (+0x236, FINDINGS: set while the player is
// ducked under a low ceiling; bit 1 of +0x235 mirrors it).
//
// With the area byte D_00810700 == 0x12, the height +0xB4 at least 165,
// bit 0x80 of +0x0A set and the object record +0x214 of subtype 6 (byte
// +3), the latch is forced on: +0x236 = 1, +0x235 |= 2, and in state
// +0x04 = 1 / +0x05 = 0 func_00174A50(self, 12.0) runs; returns 1.
// Otherwise a set latch is cleared and re-tested: for up to seven yaw
// offsets of D_00248950 (added to the heading +0xC4 and wrapped), the local
// point (0, 4.01, 4, 1) is turned by that yaw and moved to the position
// +0xB0 (scratch matrix D_700036A0 -> D_700038A0), and the first
// func_001760C0(self, point, 1, 13.99) that returns nonzero sets the latch
// again. Then +0x235 gets bit 1 while the latch is set, or keeps only bit
// 0; in state +0x04 = 1 / +0x05 = 0 a change of the latch runs
// func_00174A50(self, 12.0). Returns 0.
extern float func_001B1470(float);
extern void func_001029C0(void *);
extern void func_00102BB0(void *, void *, float);
extern void func_00102918(void *, void *, char *);
extern void func_001026A0(void *, void *, float *);
extern int func_001760C0(char *, void *, int, float);
extern void func_00174A50(char *, float);
extern float D_00248950;
extern unsigned char D_00810700;
extern int D_700036A0;
extern int D_700038A0;

int func_001756E0(char *self) {
    float local_point[4];
    int i;
    int old;
    float *yaw_offset;

    if (D_00810700 == 0x12 && !(*(float *)(self + 0xB4) < 165.0f) &&
        (*(unsigned char *)(self + 0xA) & 0x80) &&
        *(unsigned char *)(*(char **)(self + 0x214) + 3) == 6) {
        *(unsigned char *)(self + 0x236) = 1;
        *(unsigned char *)(self + 0x235) = *(unsigned char *)(self + 0x235) | 2;
        if (*(unsigned char *)(self + 4) == 1) {
            if (*(unsigned char *)(self + 5) == 0) {
                func_00174A50(self, 12.0f);
            }
        }
        return 1;
    }
    old = *(unsigned char *)(self + 0x236);
    if (old != 0) {
        *(unsigned char *)(self + 0x236) = 0;
        local_point[0] = 0.0f;
        local_point[1] = 4.01f;
        local_point[2] = 4.0f;
        local_point[3] = 1.0f;
        yaw_offset = &D_00248950;
        i = 0;
        do {
            func_001029C0(&D_700036A0);
            func_00102BB0(&D_700036A0, &D_700036A0, func_001B1470(*(float *)(self + 0xC4) + *yaw_offset));
            func_00102918(&D_700036A0, &D_700036A0, self + 0xB0);
            func_001026A0(&D_700038A0, &D_700036A0, local_point);
            if (func_001760C0(self, &D_700038A0, 1, 13.99f) != 0) {
                *(unsigned char *)(self + 0x236) = 1;
                break;
            }
            i += 1;
            yaw_offset += 1;
        } while (i < 7);
    }
    if (*(unsigned char *)(self + 0x236) != 0) {
        *(unsigned char *)(self + 0x235) = *(unsigned char *)(self + 0x235) | 2;
    } else {
        *(unsigned char *)(self + 0x235) = *(unsigned char *)(self + 0x235) & 1;
    }
    if (*(unsigned char *)(self + 4) == 1) {
        if (*(unsigned char *)(self + 5) == 0 && old != *(unsigned char *)(self + 0x236)) {
            func_00174A50(self, 12.0f);
        }
    }
    return 0;
}
