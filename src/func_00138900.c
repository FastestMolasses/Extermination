// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// SEMANTICS: per-frame behaviour step for an actor (arg0) driven by its
//   controller block (arg1), dispatched on the sub-state byte arg0[6].
//   arg1+0x20 (u16) = countdown/latch, arg1+0x44/0x48 = speed + accel,
//   arg1+0x50/0x5C = current + goal lean angle, arg1+0x70 = event word
//   (bit 0x1000 = "clip finished"), arg1+0x81 = input/contact flags,
//   arg0+0x3C = clip frame counter, arg0+0xB0 = position, arg0+0xB4 = height.
//   state 0: tick the countdown down; when it expires either bail to state 4
//            (if arg0[0xD] bit 0 is clear) or advance, clamp the speed, set
//            the goal lean to +pi/2 and play clip 4.
//   state 1: clamp speed/accel to zero, and on contact flag 4 advance, reset
//            the lean/speed state, drop 2.5 units of height and play clip 2.
//   state 2: count frames while D_008106C7 is set and the listener is within
//            150.0 (func_001B15D0), else reset the counter; when arg0[0xA]
//            bit 0 is set or the counter has ticked, advance, play clip 3 and
//            fire sound cue 0x816 at 300.0.
//   state 3: past frame 25.0 flip the goal lean to -pi/2; on the advance
//            signal reset to outer state 2 / sub-state 0 and play clip 1.
//   state 4: bump the outer state, reset the sub-state and play clip 1.
//   Tail: ease the current lean toward the goal by pi/120 per frame, then run
//   the shared post-step (func_0013BA20).
//
//   MATCHING DEVICE (semantic no-op): the `(z, (z = 0.0f))` argument pair on
//   the clip-4 and clip-3 calls reproduces CW's f13-before-f12 float-argument
//   register order in those two arms only (idiom-23/24, see
//   docs/fanout/MATCHING_GUIDE.md). The other three call sites really do emit
//   f12 first and must keep the plain `0.0f, 0.0f` spelling.

extern void anim_clip_init(unsigned char *, int, float, float);
extern void func_0013BA20(unsigned char *, unsigned char *);
extern void func_001FBD50(unsigned char *, int, int, float);
extern float func_001B12B0(float, float, float);
extern float func_001B15D0(unsigned char *, float *);

extern unsigned char D_008106C7;
extern float D_00810360;

void func_00138900(unsigned char *arg0, unsigned char *arg1) {
    unsigned char st;
    float z;

    st = *(unsigned char *)(arg0 + 6);
    switch (st) {
    case 0:
        if (*(unsigned short *)(arg1 + 0x20) != 0) {
            *(unsigned short *)(arg1 + 0x20) = *(unsigned short *)(arg1 + 0x20) - 1;
        } else if (!(*(unsigned char *)(arg0 + 0xD) & 1)) {
            *(unsigned char *)(arg0 + 6) = 4;
        } else {
            *(unsigned char *)(arg0 + 6) = st + 1;
            if (*(float *)(arg1 + 0x44) > 0.0f) {
                *(int *)(arg1 + 0x48) = 0xBC23D70A;
            } else {
                *(float *)(arg1 + 0x44) = 0.0f;
            }
            *(float *)(arg1 + 0x5C) = 1.5707964f;
            anim_clip_init(arg0, 4, z, (z = 0.0f));
        }
        break;
    case 1:
        if (*(float *)(arg1 + 0x44) <= 0.0f) {
            *(int *)(arg1 + 0x48) = 0;
            *(float *)(arg1 + 0x44) = 0.0f;
        }
        if (*(signed char *)(arg1 + 0x81) & 4) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
            *(unsigned short *)(arg1 + 0x20) = 0;
            *(float *)(arg1 + 0x50) = 0.0f;
            *(float *)(arg1 + 0x5C) = 0.0f;
            *(float *)(arg1 + 0x44) = 0.0f;
            *(int *)(arg1 + 0x48) = 0;
            *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0xB4) - 2.5f;
            anim_clip_init(arg0, 2, 0.0f, 0.0f);
        }
        break;
    case 2:
        if (D_008106C7 != 0 && func_001B15D0(arg0 + 0xB0, &D_00810360) <= 150.0f) {
            *(unsigned short *)(arg1 + 0x20) = *(unsigned short *)(arg1 + 0x20) + 1;
        } else {
            *(unsigned short *)(arg1 + 0x20) = 0;
        }
        if ((*(unsigned char *)(arg0 + 0xA) & 1) || *(unsigned short *)(arg1 + 0x20) > 0) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
            *(unsigned short *)(arg1 + 0x20) = 0;
            anim_clip_init(arg0, 3, z, (z = 0.0f));
            func_001FBD50(arg0, 0x816, 0, 300.0f);
        }
        break;
    case 3:
        if (*(float *)(arg0 + 0x3C) <= 25.0f) {
            *(float *)(arg1 + 0x5C) = -1.5707964f;
        }
        if (*(int *)(arg1 + 0x70) & 0x1000) {
            *(unsigned char *)(arg0 + 5) = 2;
            *(unsigned char *)(arg0 + 6) = 0;
            *(float *)(arg1 + 0x5C) = 0.0f;
            *(short *)(arg1 + 0x30) = 0;
            *(float *)(arg1 + 0x44) = 0.4f;
            *(short *)(arg1 + 0x22) = 0;
            *(unsigned short *)(arg1 + 0x20) = 0;
            anim_clip_init(arg0, 1, 0.0f, 0.0f);
        }
        break;
    case 4:
        *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) + 1;
        *(unsigned char *)(arg0 + 6) = 0;
        *(float *)(arg1 + 0x5C) = 0.0f;
        *(short *)(arg1 + 0x30) = 0;
        *(float *)(arg1 + 0x44) = 0.4f;
        *(short *)(arg1 + 0x24) = 0;
        *(short *)(arg1 + 0x22) = 0;
        *(unsigned short *)(arg1 + 0x20) = 0;
        anim_clip_init(arg0, 1, 0.0f, 0.0f);
        break;
    }
    *(float *)(arg1 + 0x50) = func_001B12B0(*(float *)(arg1 + 0x5C), *(float *)(arg1 + 0x50), 0.026179939f);
    func_0013BA20(arg0, arg1);
}
