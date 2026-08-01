// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Per-state actor driver dispatched on the state byte at arg0+6 (4-way switch
// 0/1/2/3), with a common tail func_00179880(arg0, arg0+0x2EC) +
// func_00175900(arg0, 1) run for every state.
//   state 0: advance state, clear arg0+7, kick clip 0x1C4 via func_001749A0,
//            register func_001EFE00(0x80000051, self), trigger
//            func_001B61C0(0, 0xC0, 5, 1), fire two func_001FBD50 sound/anim
//            cues (0x146 and 0x151 at 300.0f), clear the timer at arg0+0x2EC.
//   state 1: once the float at arg0+0x3C has dropped to <= 160.0, advance
//            state, set arg0+0x234 = 2 and the global D_00810707 = 2, and call
//            func_0015C1F0(arg0); always run func_0021E650(arg0) afterward.
//   state 2: if the 0x1000 flag is set in arg0+0x200, advance state, clear
//            arg0+7 and trigger func_001B61C0(1, 0xEE, 0x3C, 1); else just run
//            func_0021E650(arg0).
//   state 3: func_0021D2E0(arg0, 0x78, 1).
//
// Note: func_0015C1F0 takes the actor pointer (see src/func_0015C1F0.c).
// Passing it is load-bearing, not cosmetic: it keeps $a0 - which still holds
// the incoming arg0 - live across the state-1 guard, so mwcc must emit the
// BRANCH-LIKELY `bc1fl` whose delay slot speculates `a0 = s0` for the
// following func_0021E650 call. With the argument dropped, mwcc is free to
// clobber $a0 in a plain `bc1f` delay slot and the function stalls at 99.38%.
extern void func_001749A0(unsigned char *e, short clip, int flags, float blend);
extern void func_001EFE00(int a, unsigned char *e);
extern void func_001B61C0(int a, int b, int c, int d);
extern void func_001FBD50(unsigned char *e, int a, int b, float f);
extern void func_0015C1F0(unsigned char *e);
extern void func_0021E650(unsigned char *e);
extern void func_0021D2E0(unsigned char *e, short a, int b);
extern void func_00179880(unsigned char *e, void *p);
extern void func_00175900(unsigned char *e, int f);
extern unsigned char D_00810707;

void func_0021E830(unsigned char *arg0) {
    unsigned char st;

    st = *(unsigned char *)(arg0 + 6);
    switch (st) {
    case 0:
        *(unsigned char *)(arg0 + 6) = st + 1;
        *(unsigned char *)(arg0 + 7) = 0;
        func_001749A0(arg0, 0x1C4, 0, 0.0f);
        func_001EFE00(0x80000051, arg0);
        func_001B61C0(0, 0xC0, 5, 1);
        func_001FBD50(arg0, 0x146, 0, 300.0f);
        func_001FBD50(arg0, 0x151, 0, 300.0f);
        *(int *)(arg0 + 0x2EC) = 0;
        break;
    case 1:
        if (*(float *)(arg0 + 0x3C) <= 160.0f) {
            *(unsigned char *)(arg0 + 6) = st + 1;
            *(unsigned char *)(arg0 + 0x234) = 2;
            D_00810707 = 2;
            func_0015C1F0(arg0);
        }
        func_0021E650(arg0);
        break;
    case 2:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            *(unsigned char *)(arg0 + 6) = st + 1;
            *(unsigned char *)(arg0 + 7) = 0;
            func_001B61C0(1, 0xEE, 0x3C, 1);
        } else {
            func_0021E650(arg0);
        }
        break;
    case 3:
        func_0021D2E0(arg0, 0x78, 1);
        break;
    }
    func_00179880(arg0, arg0 + 0x2EC);
    func_00175900(arg0, 1);
}
