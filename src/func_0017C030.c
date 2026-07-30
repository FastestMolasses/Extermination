// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// SEMANTICS: per-frame "special move / knock-down" sub-state machine for an
// actor (p = the actor block, same layout as func_0017B490/func_001749A0's
// first argument).  p[0x1F0] is the sub-state id (0..7, 8-entry jump table;
// states 0 and 2 do nothing) and p[0x25C] is the move/phase code driving the
// individual states.
//   p[0x1F0]  = sub-state id (jump-table selector)
//   p[0x1F1]  = move variant (3 = the "alternate" clip set)
//   p[0x200]  = input/condition flag word; bit 0x1000 is the gate used to
//               leave states 4, 5 and 6
//   p[0x204]  = float blend/speed scalar written as a raw constant
//               (2.0f while sliding, 0.75f on the normal exit)
//   p[0x235]  = clip-set index handed to func_0017B490
//   p[0x23F]  = retry counter; >1 sets p[6] = 0x63 and calls func_0017C440
//   p[0x25C]  = phase code (0 = none, 1 = sliding, 3 = clip-driven)
//   p[0x25E]  = result/exit code reported to the caller (0x81/0x82/0x83)
//   p[0x260], p[0x264] = per-frame world-space X/Z deltas added to the actor
//               position p[0xB0]/p[0xB8]
//   p[0x268]  = remaining slide time in frames, counted down by 1.0f
//   p[0x38]   = scratch word cleared on state entry/exit
//   p[0xC4]   = facing angle, rotated by pi through func_001B1470
//   0x70003A20 = EE scratchpad float temp used to stage func_001C61D0's
//               result before subtracting 6.0f for anim_clip_arbiter

extern void anim_matrix_player(unsigned char *);
extern short func_0017B490(unsigned char *, int, int, int);
extern int func_001C61D0(int, int);
extern void anim_clip_arbiter(unsigned char *, int, float, float);
extern void func_0017B910(unsigned char *);
extern void func_0017C440(unsigned char *, int);
extern void func_001749A0(unsigned char *, short, int, float);
extern void func_001FB9F0(int, int, int, int);
extern float func_001B1470(float);

void func_0017C030(unsigned char *p)
{
    unsigned char sub;
    short s;
    int n;

    switch (p[0x1F0]) {
    case 0:
        break;
    case 1:
        if (p[0x25C] != 0) {
            anim_matrix_player(p);
        }
        break;
    case 2:
        break;
    case 3:
        sub = p[0x25C];
        if (sub == 3) {
            s = func_0017B490(p, 6, p[0x235], sub);
            *(volatile float *)0x70003A20 = (float)func_001C61D0(*(int *)(p + 0x40), s);
            anim_clip_arbiter(p, s, 6.0f, *(volatile float *)0x70003A20 - 6.0f);
            p[0x1F0] = 4;
        } else {
            func_0017B910(p);
        }
        *(int *)(p + 0x38) = 0;
        break;
    case 4:
        n = p[0x23F];
        if (n > 1) {
            p[6] = 0x63;
            func_0017C440(p, 1);
        } else if (*(int *)(p + 0x200) & 0x1000) {
            p[0x1F0] = 0;
            p[0x25C] = 0;
            p[0x25E] = 0x83;
        }
        break;
    case 5:
        if (p[0x25C] == 1) {
            if (*(float *)(p + 0x268) < 1.0f) {
                p[0x1F0] = 0;
                p[0x25C] = 0;
                p[0x25E] = 0x81;
            } else {
                *(float *)(p + 0xB0) += *(float *)(p + 0x260);
                *(float *)(p + 0xB8) += *(float *)(p + 0x264);
                *(float *)(p + 0x268) -= 1.0f;
                *(float *)(p + 0x204) = 2.0f;
            }
        } else if (*(float *)(p + 0x268) < 1.0f) {
            if (*(int *)(p + 0x200) & 0x1000) {
                p[0x1F0] = 0;
                p[0x25C] = 0;
                p[0x25E] = 0x82;
            }
        } else {
            *(float *)(p + 0xB0) += *(float *)(p + 0x260);
            *(float *)(p + 0xB8) += *(float *)(p + 0x264);
            *(float *)(p + 0x268) -= 1.0f;
        }
        break;
    case 7:
        if (p[0x1F1] == 3) {
            s = func_0017B490(p, 2, p[0x235], 0);
        } else {
            s = func_0017B490(p, 4, p[0x235], 0);
        }
        func_001749A0(p, s, 0, 4.0f);
        p[0x1F0] = 6;
        func_001FB9F0(0x137, 0x1000, 0x1000, 0x1000);
        break;
    case 6:
        if (*(int *)(p + 0x200) & 0x1000) {
            if (p[0x1F1] == 3) {
                s = func_0017B490(p, 3, p[0x235], 0);
            } else {
                s = func_0017B490(p, 5, p[0x235], 0);
            }
            func_001749A0(p, s, 1, 0.0f);
            p[0x1F0] = 0;
            p[0x25C] = 0;
            *(int *)(p + 0x38) = 0;
            *(float *)(p + 0xC4) = func_001B1470(3.1415927f + *(float *)(p + 0xC4));
        } else {
            *(float *)(p + 0x204) = 0.75f;
        }
        break;
    }
}
