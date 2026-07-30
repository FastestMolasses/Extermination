// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// SEMANTICS: per-state driver for entity arg0, dispatched on the state byte at
// arg0+6 (jump table jtbl_002737F0, 9 states); out-of-range states do nothing.
// Sibling of func_0021EF30 — same state machine, but states 4-7 additionally run
// the func_00179880 / func_00175900 step-and-advance tail on every path.
// arg0+0x200 is the input/event bitfield (0x1000 advance, 0x8000 hold), arg0+0x21C
// / arg0+0x2E4 cache the previous world time / position read from D_00275B40[0],
// and arg0+0x38 / arg0+0xB4 / arg0+0x2EC carry the per-frame deltas.
// State 0 fires sound 0x159, cue func_001B61C0(1, 0xEE, 0x3C, 1), advances, and
// picks clip 0x26 or 0x22 depending on func_0021D1A0 (latched in arg0+0xD).
// State 1 builds the quaternion at scratchpad 0x700038B0 from the yaw at arg0+0xC4
// rotated by pi and submits it for bones D_00275B40[7], [2] and [3], then falls
// through into state 2, which steps the animation at 0.75 rate until the advance
// bit appears. State 3 plays out the move, then fires clip 0x23/0x27 and sound
// 0x12F. State 4 steps at 0.5 rate until the advance bit, then fires clip 0x25/0x29
// at blend 4.0. States 5-7 are timed gates (hold bit, arg0+0x3C <= 18, advance bit)
// and state 8 tail-calls func_0021D2E0(arg0, 0x78, 0).
extern void func_001749A0(char *p, int a, int b, float f);
extern int func_00175900(char *p, int a);
extern void func_00178B90(char *p, int a);
extern void func_00179880(char *p, char *q);
extern float func_001B1470(float);
extern void func_001B61C0(int a, int b, int c, int d);
extern void func_001EFD90(int a, void *b, void *c);
extern void func_001FBD50(char *p, int a, int b, float f);
extern int func_0021D1A0(char *p);
extern void func_0021D2E0(char *p, int a, int b);
extern void func_0021D490(char *p);
extern int *D_00275B40;
extern char D_700038B0[];

void func_0021EAD0(char *arg0) {
    unsigned char st;

    st = *(unsigned char *)(arg0 + 6);
    switch (st) {
    case 0:
        func_001FBD50(arg0, 0x159, 0, 300.0f);
        func_001B61C0(1, 0xEE, 0x3C, 1);
        *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
        if (func_0021D1A0(arg0) != 0) {
            *(unsigned char *)(arg0 + 0xD) = 1;
            func_001749A0(arg0, 0x26, 0, 1.0f);
        } else {
            *(unsigned char *)(arg0 + 0xD) = 0;
            func_001749A0(arg0, 0x22, 0, 1.0f);
        }
        *(float *)(arg0 + 0x21C) = 0.0f;
        *(float *)(arg0 + 0x2E4) = 0.0f;
        *(float *)(arg0 + 0x2EC) = 0.0f;
        break;
    case 1:
        *(unsigned char *)(arg0 + 6) = st + 1;
        *(int *)0x700038B0 = 0;
        *(float *)0x700038B4 = func_001B1470(3.1415927f + *(float *)(arg0 + 0xC4));
        *(int *)0x700038B8 = 0;
        *(float *)0x700038BC = 1.0f;
        func_001EFD90(0x80000023, (char *)D_00275B40[7] + 0xC0, D_700038B0);
        func_001EFD90(0x80000023, (char *)D_00275B40[2] + 0xC0, D_700038B0);
        func_001EFD90(0x80000023, (char *)D_00275B40[3] + 0xC0, D_700038B0);
        /* fallthrough */
    case 2:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
            break;
        }
        *(float *)(arg0 + 0x38) = *(float *)(D_00275B40[0] + 8) - *(float *)(arg0 + 0x21C);
        *(float *)(arg0 + 0x21C) = *(float *)(D_00275B40[0] + 8);
        *(float *)(arg0 + 0x38) *= 0.75f;
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
            if (*(unsigned char *)(arg0 + 0xD) == 0) {
                func_001749A0(arg0, 0x23, 0, 0.0f);
            } else {
                func_001749A0(arg0, 0x27, 0, 0.0f);
            }
            *(float *)(arg0 + 0x21C) = 0.0f;
            *(float *)(arg0 + 0x2E4) = 0.0f;
            func_001FBD50(arg0, 0x12F, 0, 300.0f);
        }
        break;
    case 4:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            *(unsigned char *)(arg0 + 6) = st + 1;
            if (*(unsigned char *)(arg0 + 0xD) == 0) {
                func_001749A0(arg0, 0x25, 0, 4.0f);
            } else {
                func_001749A0(arg0, 0x29, 0, 4.0f);
            }
        } else {
            *(float *)(arg0 + 0x38) = *(float *)(D_00275B40[0] + 8) - *(float *)(arg0 + 0x21C);
            *(float *)(arg0 + 0x21C) = *(float *)(D_00275B40[0] + 8);
            *(float *)(arg0 + 0x38) *= 0.5f;
            func_00178B90(arg0, 1);
        }
        func_00179880(arg0, arg0 + 0x2EC);
        func_00175900(arg0, 1);
        break;
    case 5:
        if (!(*(int *)(arg0 + 0x200) & 0x8000)) {
            *(unsigned char *)(arg0 + 6) = st + 1;
        }
        func_00179880(arg0, arg0 + 0x2EC);
        func_00175900(arg0, 1);
        break;
    case 6:
        if (*(float *)(arg0 + 0x3C) <= 18.0f) {
            *(unsigned char *)(arg0 + 6) = st + 1;
            func_0021D490(arg0);
        }
        func_00179880(arg0, arg0 + 0x2EC);
        func_00175900(arg0, 1);
        break;
    case 7:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            *(unsigned char *)(arg0 + 6) = st + 1;
            *(char *)(arg0 + 7) = 0;
        }
        func_00179880(arg0, arg0 + 0x2EC);
        func_00175900(arg0, 1);
        break;
    case 8:
        func_0021D2E0(arg0, 0x78, 0);
        break;
    }
}
