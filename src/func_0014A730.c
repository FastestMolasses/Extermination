// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 95.84% NEARMISS).
// Fixes read from the original instructions:
//  - The pose-select tail (both arms) is an if/else with a separate store of
//    8 or 6 to arg0+5 in each branch, not a temp defaulted to 6 and
//    overwritten; the default-then-overwrite spelling coloured the temp into a
//    different register and merged the stores.
//
// 3-state switch/fallthrough dispatcher on arg0+6: state 0 resets arg1's
// fields and starts a clip (0x30 when arg1+0x87 is set, 0x2E with a
// func_001FBD50 cue when arg0+0x36 has bit 15, otherwise a random clip from
// D_00245BD0); state 1 waits for arg1+0x20 bit 0x1000; state 2 also clears
// arg1+0x87 once arg0+0x3C drops to 45 or below.

extern void anim_clip_init(char *self, int clip, float a, float b);
extern void func_001FBD50(char *p, int a, int b, float f);
extern int func_00122BB8(void);
extern short D_00245BD0[][2];

void func_0014A730(char *arg0, char *arg1) {
    unsigned char state;

    state = *(unsigned char *)(arg0 + 6);
    switch (state) {
    case 0:
        *(unsigned char *)(arg0 + 6) = state + 1;
        *(int *)(arg1 + 0x40) = 0x3F800000;
        *(char *)(arg1 + 0x88) = 0;
        *(char *)(arg1 + 0x83) = 0;
        *(int *)(arg1 + 0x50) = 0;
        *(int *)(arg1 + 0x4C) = 0;
        if (*(char *)(arg1 + 0x87) != 0) {
            anim_clip_init(arg0, 0x30, 1.0f, 0.0f);
        } else if (*(short *)(arg0 + 0x36) & 0x8000) {
            func_001FBD50(arg0, 0x7E9, 0, 300.0f);
            anim_clip_init(arg0, 0x2E, 1.0f, 0.0f);
        } else {
            *(char *)(arg0 + 0) = 1;
            *(short *)(arg0 + 0x36) = 0;
            *(char *)(arg1 + 0x85) = 0;
            func_001FBD50(arg0, 0x7E7, 0, 300.0f);
            anim_clip_init(arg0, D_00245BD0[(func_00122BB8() >> 0x11) % 3][0], 1.0f, 0.0f);
        }
        /* fallthrough */
    case 1:
        if (*(int *)(arg1 + 0x20) & 0x1000) {
            if (*(char *)(arg1 + 0x87) != 0) {
                *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
                anim_clip_init(arg0, 0x2D, 0.0f, 0.0f);
                return;
            }
            *(char *)(arg0 + 4) = 1;
            *(unsigned char *)(arg0 + 6) = 0;
            *(char *)(arg0 + 0) = 1;
            *(short *)(arg1 + 0x72) = 0;
            *(short *)(arg0 + 0x36) = 0;
            *(char *)(arg1 + 0x85) = 0x3C;
            if (*(float *)(arg1 + 0x68) <= 20.0f) {
                *(char *)(arg0 + 5) = 8;
            } else {
                *(char *)(arg0 + 5) = 6;
            }
        }
        break;
    case 2:
        if (*(float *)(arg0 + 0x3C) <= 45.0f) {
            *(char *)(arg1 + 0x87) = 0;
        }
        if (*(int *)(arg1 + 0x20) & 0x1000) {
            *(char *)(arg0 + 4) = 1;
            *(unsigned char *)(arg0 + 6) = 0;
            *(char *)(arg1 + 0x87) = 0;
            *(char *)(arg0 + 0) = 1;
            *(short *)(arg1 + 0x72) = 0;
            *(short *)(arg0 + 0x36) = 0;
            *(char *)(arg1 + 0x85) = 0x3C;
            if (*(float *)(arg1 + 0x68) <= 20.0f) {
                *(char *)(arg0 + 5) = 8;
            } else {
                *(char *)(arg0 + 5) = 6;
            }
        }
        break;
    }
}
