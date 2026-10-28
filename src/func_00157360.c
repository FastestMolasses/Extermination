// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 91.71% NEARMISS).
// Fixes read from the original instructions:
//  - func_00102948 takes untyped quadword pointers (void *dst, void *src);
//    the (int *) casts on the arg2 sources reversed the argument setup order.
//  - D_008105D0 is one 8-word block (two 4-word keyframes, the second at
//    D_008105D0[4] = 0x008105E0). With two separate arrays mwcc hoisted the
//    shared 0x43A78000 store to the second keyframe; one aggregate keeps the
//    original store order.
//  - The timers are compound `+= 1.0f` on the field with a `> K` advance test,
//    the region flag is `D_00810841[D_00810700] |= ...`, and every case ends in
//    `break` to one shared `return 0`.
//
// A short state machine (state byte at arg1+4) driving a scripted camera/effect
// intro. arg2 is the camera record; arg2+0x10 is a float timer.
//   state 0: build two camera keyframe sets. If the region id D_00810700 == 7 use
//     the disc-authored presets (four func_00102948 copies from D_008105D0/E0 and
//     D_008101F0/0200 into arg2+0x20/+0x30); otherwise write the default keyframe
//     floats into D_008105D0..EC (43A78000, 436F0000, 44117333, 1.0f / 43A78000,
//     436D0000, 4405C000, 1.0f) and load them via func_00102948. Then zero the timer
//     and advance the state.
//   state 1: advance the timer by 1.0; once it passes 30.0, advance the state.
//   state 2: set bit (arg0+0x2E) in the per-region flag byte D_00810841[D_00810700],
//     fire func_001FB9F0(0x3EE, 0x1000, 0x1000, 0x1000) (a sound/rumble cue), zero
//     the timer, advance the state.
//   state 3: advance the timer by 1.0; once it passes 60.0, advance the state.
//   state 4: func_001B0460(1) and return 1 (sequence finished). All other states 0.
extern void func_00102948(void *dst, void *src);
extern void func_001B0460(int a);
extern void func_001FB9F0(int a, int b, int c, int d);
extern int D_008101F0[4];
extern int D_00810200[4];
extern int D_008105D0[8];
extern unsigned char D_00810700;
extern unsigned char D_00810841[];

int func_00157360(char *arg0, char *arg1, char *arg2) {
    switch (*(unsigned char *)(arg1 + 4)) {
    case 0:
        if (D_00810700 == 7) {
            func_00102948(D_008105D0, arg2 + 0x20);
            func_00102948(&D_008105D0[4], arg2 + 0x30);
            func_00102948(D_008101F0, arg2 + 0x20);
            func_00102948(D_00810200, arg2 + 0x30);
        } else {
            D_008105D0[0] = 0x43A78000;
            D_008105D0[1] = 0x436F0000;
            D_008105D0[2] = 0x44117333;
            D_008105D0[3] = 0x3F800000;
            D_008105D0[4] = 0x43A78000;
            D_008105D0[5] = 0x436D0000;
            D_008105D0[6] = 0x4405C000;
            D_008105D0[7] = 0x3F800000;
            func_00102948(D_008101F0, D_008105D0);
            func_00102948(D_00810200, &D_008105D0[4]);
        }
        *(int *)(arg2 + 0x10) = 0;
        *(unsigned char *)(arg1 + 4) += 1;
        break;
    case 1:
        *(float *)(arg2 + 0x10) += 1.0f;
        if (*(float *)(arg2 + 0x10) > 30.0f) {
            *(unsigned char *)(arg1 + 4) += 1;
        }
        break;
    case 2:
        D_00810841[D_00810700] |= 1 << *(unsigned char *)(arg0 + 0x2E);
        func_001FB9F0(0x3EE, 0x1000, 0x1000, 0x1000);
        *(int *)(arg2 + 0x10) = 0;
        *(unsigned char *)(arg1 + 4) += 1;
        break;
    case 3:
        *(float *)(arg2 + 0x10) += 1.0f;
        if (*(float *)(arg2 + 0x10) > 60.0f) {
            *(unsigned char *)(arg1 + 4) += 1;
        }
        break;
    case 4:
        func_001B0460(1);
        return 1;
    }
    return 0;
}
