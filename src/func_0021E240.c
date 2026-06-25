// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
//
// Per-state behavior dispatcher (state byte at arg0+6, a 3-way switch 0/1/2).
// State 0: kicks effect func_001B61C0(0,0xC0,5,1), fires two func_001FBD50 clips
// (0x146, 0x151 at 300.0f), bumps the state, zeroes the sub-state at arg0+7,
// then arms anim func_001749A0 (clip 0x5C if the flag at arg0+0x236 is set, else
// clip 0x2A) at 1.0f, and clears the floats at arg0+0x21C / arg0+0x38 and the int
// at arg0+0x2EC. State 1: if the 0x1000 bit in arg0+0x200 is set, advance state &
// reset sub-state; otherwise run a nested 3-way sub-state machine (arg0+7) gated on
// the countdown float at arg0+0x3C (<=80, <=50, <=16) that arms clips / calls
// func_00182870 / func_0021D490+func_001B61C0 as it steps, then updates the delta
// float arg0+0x38 = (*D_00275B40)[2] - arg0+0x21C, stores the new time into
// arg0+0x21C, and calls func_00178B90(self,1). State 2: delegates to func_0021D2E0.
// All states fall through to the common tail func_00179880(self, self+0x2EC) +
// func_00175900(self, 1).
//
// Matched with mwcc 2.3.3 (mwcps2-2.3.3-000906); the pinned 991202 build reaches
// 81.7%. Keys: writing the dispatch (both the outer and the inner state) as a
// `switch` on a local-loaded byte reproduces the target's descending compare order
// and case-0 fill; -sdatathreshold 8 keeps D_00275B40 gp-relative as in the target.
// objdiff 100.0 vs expected.
extern void func_001749A0(unsigned char *e, int clip, int flags, float blend);
extern int func_00175900(unsigned char *e, int f);
extern void func_00178B90(unsigned char *e, int f);
extern void func_00179880(unsigned char *e, void *p);
extern void func_00182870(unsigned char *e, int f);
extern void func_001B61C0(int a, int b, int c, int d);
extern void func_001FBD50(unsigned char *p, int a, int b, float f);
extern void func_0021D2E0(unsigned char *e, short a, int b);
extern void func_0021D490(unsigned char *e);
extern unsigned char **D_00275B40;

void func_0021E240(unsigned char *arg0) {
    unsigned char st;
    unsigned char st2;

    st = *(unsigned char *)(arg0 + 6);
    switch (st) {
    case 0:
        func_001B61C0(0, 0xC0, 5, 1);
        func_001FBD50(arg0, 0x146, 0, 300.0f);
        func_001FBD50(arg0, 0x151, 0, 300.0f);
        *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
        *(unsigned char *)(arg0 + 7) = 0;
        if (*(unsigned char *)(arg0 + 0x236) != 0) {
            func_001749A0(arg0, 0x5C, 0, 1.0f);
        } else {
            func_001749A0(arg0, 0x2A, 0, 1.0f);
        }
        *(int *)(arg0 + 0x21C) = 0;
        *(int *)(arg0 + 0x38) = 0;
        *(int *)(arg0 + 0x2EC) = 0;
        break;
    case 1:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            *(unsigned char *)(arg0 + 6) = st + 1;
            *(unsigned char *)(arg0 + 7) = 0;
        } else {
            st2 = *(unsigned char *)(arg0 + 7);
            switch (st2) {
            case 0:
                if (*(float *)(arg0 + 0x3C) <= 80.0f) {
                    *(unsigned char *)(arg0 + 7) = st2 + 1;
                    func_001FBD50(arg0, 0x156, 0, 300.0f);
                }
                break;
            case 1:
                if (*(float *)(arg0 + 0x3C) <= 50.0f) {
                    *(unsigned char *)(arg0 + 7) = st2 + 1;
                    func_00182870(arg0, 1);
                }
                break;
            case 2:
                if (*(float *)(arg0 + 0x3C) <= 16.0f) {
                    *(unsigned char *)(arg0 + 7) = st2 + 1;
                    func_0021D490(arg0);
                    func_001B61C0(1, 0xEE, 0x3C, 1);
                }
                break;
            }
            *(float *)(arg0 + 0x38) = *(float *)(*D_00275B40 + 8) - *(float *)(arg0 + 0x21C);
            *(float *)(arg0 + 0x21C) = *(float *)(*D_00275B40 + 8);
            func_00178B90(arg0, 1);
        }
        break;
    case 2:
        func_0021D2E0(arg0, 0x78, 1);
        break;
    }
    func_00179880(arg0, arg0 + 0x2EC);
    func_00175900(arg0, 1);
}
