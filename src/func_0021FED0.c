// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// SEMANTICS: per-state driver for entity arg0, dispatched on the state byte at
// arg0+6 (jump table jtbl_00273880, 7 states). arg0+0x200 holds the input/event
// bitfield (0x1000 = advance trigger, 0x8000 = hold), arg0+0x21C / arg0+0x2E4 are
// the previous world-time / world-position samples taken from D_00275B40[0], and
// arg0+0x38 / arg0+0xB4 / arg0+0x2EC are the per-frame deltas fed to the animation
// stepper func_00178B90 / mover func_00179880 / advancer func_00175900.
// State 0 arms clip 0x32. State 1 waits for the trigger, then aims the camera clip
// via func_001C61D0/anim_clip_arbiter; otherwise it just advances at full rate.
// State 2 is the same wait but decays the step to 0.4. State 3 runs the animation
// to completion, then fires clip 0x12F, cue func_001B61C0(0, 0xD0, 0xA, 1) and clip
// 0x27. States 4-6 gate on the trigger (state 4 kicks clip 0x28, state 5 clears
// arg0+0x1F1, state 6 sets the 0x3C-frame timer at arg0+0x20E, clears arg0+0x25C
// and calls func_0017C540) and otherwise keep stepping. Every state ends by
// handing off to func_0021D250 when the script id at arg0+0x23A is 0x5D.
extern void func_001749A0(char *p, int a, int b, float f);
extern int func_00175900(char *p, int a);
extern void func_00178B90(char *p, int a);
extern void func_00179880(char *p, char *q);
extern void anim_clip_arbiter(char *p, int clip, float a, float b);
extern void func_0017C540(char *p);
extern void func_001B61C0(int a, int b, int c, int d);
extern int func_001C61D0(int a, int b);
extern void func_001FBD50(char *p, int a, int b, float f);
extern void func_0021D250(char *p, int a);
extern int *D_00275B40;

void func_0021FED0(char *arg0) {
    unsigned char st;

    st = *(unsigned char *)(arg0 + 6);
    switch (st) {
    case 0:
        *(unsigned char *)(arg0 + 6) += 1;
        *(char *)(arg0 + 7) = 0;
        func_001749A0(arg0, 0x32, 0, 1.0f);
        *(float *)(arg0 + 0x38) = 0.0f;
        *(float *)(arg0 + 0x21C) = 0.0f;
        break;
    case 1:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            *(unsigned char *)(arg0 + 6) = st + 1;
            *(float *)0x70003A20 = (float)func_001C61D0(*(int *)(arg0 + 0x40), 0x26);
            anim_clip_arbiter(arg0, 0x26, 0.0f, *(float *)0x70003A20 - 30.0f);
            *(float *)(arg0 + 0x38) = 0.0f;
            *(float *)(arg0 + 0x21C) = *(float *)(D_00275B40[0] + 8);
            *(float *)(arg0 + 0x2E4) = 0.0f;
            break;
        }
        *(float *)(arg0 + 0x38) = *(float *)(D_00275B40[0] + 8) - *(float *)(arg0 + 0x21C);
        *(float *)(arg0 + 0x21C) = *(float *)(D_00275B40[0] + 8);
        func_00178B90(arg0, 1);
        *(float *)(arg0 + 0x2EC) = *(float *)(D_00275B40[0] + 4) - *(float *)(arg0 + 0x2E4);
        *(float *)(arg0 + 0x2E4) = *(float *)(D_00275B40[0] + 4);
        *(float *)(arg0 + 0xB4) += *(float *)(arg0 + 0x2EC);
        func_00175900(arg0, 1);
        break;
    case 2:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            *(unsigned char *)(arg0 + 6) = st + 1;
            break;
        }
        *(float *)(arg0 + 0x38) = *(float *)(D_00275B40[0] + 8) - *(float *)(arg0 + 0x21C);
        *(float *)(arg0 + 0x21C) = *(float *)(D_00275B40[0] + 8);
        *(float *)(arg0 + 0x38) *= 0.4f;
        func_00178B90(arg0, 1);
        *(float *)(arg0 + 0x2EC) = *(float *)(D_00275B40[0] + 4) - *(float *)(arg0 + 0x2E4);
        *(float *)(arg0 + 0x2E4) = *(float *)(D_00275B40[0] + 4);
        *(float *)(arg0 + 0xB4) += *(float *)(arg0 + 0x2EC);
        func_00175900(arg0, 1);
        break;
    case 3:
        func_00178B90(arg0, 1);
        func_00179880(arg0, arg0 + 0x2EC);
        if (func_00175900(arg0, 1) != 0) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
            func_001FBD50(arg0, 0x12F, 0, 300.0f);
            func_001B61C0(0, 0xD0, 0xA, 1);
            func_001749A0(arg0, 0x27, 0, 0.0f);
            *(float *)(arg0 + 0x38) = 0.0f;
            *(float *)(arg0 + 0x21C) = 0.0f;
        }
        break;
    case 4:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            *(unsigned char *)(arg0 + 6) = st + 1;
            func_001749A0(arg0, 0x28, 0, 12.0f);
            *(float *)(arg0 + 0x38) = 0.0f;
            *(float *)(arg0 + 0x21C) = 0.0f;
        } else {
            *(float *)(arg0 + 0x38) = *(float *)(D_00275B40[0] + 8) - *(float *)(arg0 + 0x21C);
            *(float *)(arg0 + 0x21C) = *(float *)(D_00275B40[0] + 8);
            func_00178B90(arg0, 1);
        }
        func_00179880(arg0, arg0 + 0x2EC);
        func_00175900(arg0, 1);
        break;
    case 5:
        if (!(*(int *)(arg0 + 0x200) & 0x8000)) {
            *(unsigned char *)(arg0 + 6) = st + 1;
            *(char *)(arg0 + 0x1F1) = 0;
        }
        func_00179880(arg0, arg0 + 0x2EC);
        func_00175900(arg0, 1);
        break;
    case 6:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            *(short *)(arg0 + 0x20E) = 0x3C;
            *(char *)(arg0 + 0x25C) = 0;
            func_0017C540(arg0);
        } else {
            *(float *)(arg0 + 0x38) = *(float *)(D_00275B40[0] + 8) - *(float *)(arg0 + 0x21C);
            *(float *)(arg0 + 0x21C) = *(float *)(D_00275B40[0] + 8);
            func_00178B90(arg0, 1);
        }
        func_00179880(arg0, arg0 + 0x2EC);
        func_00175900(arg0, 1);
        break;
    }
    if (*(unsigned char *)(arg0 + 0x23A) == 0x5D) {
        func_0021D250(arg0, 0);
    }
}
