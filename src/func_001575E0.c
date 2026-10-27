// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x700038A0 0x700038A4 0x700038A8 0x700038AC

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 92.28% NEARMISS).
// Fixes read from the original instructions:
//  - func_001B6F00 takes (owner, yaw, point): arg0 passes through in a0,
//    &D_700038A0 in a1 (twin func_001576E0 declares it so).
//  - The scratchpad words 0x700038A0..AC are relocated volatile externs
//    (idiom-32, // SPAD:), which gives the original's per-store constant
//    loads; the constants are stored as int bit patterns, like the twin.
//  - D_008105D0 and D_008105E0 are 4-float vectors (func_00157360 fills
//    both). Writing D_008105E0 as one array, all four elements (1.0f last)
//    before the second scratchpad block, reproduces the original store
//    schedule; separate scalar externs scheduled the stores differently.
//
// Sets up a point block at 0x700038A0 (10, 0, -5, 1.0) and calls
// func_001B6F00(actor, pi/2, &scratch), then writes the vector D_008105E0
// (D_00810350, 11 + D_00810354, D_00810358, 1.0) and a second point block
// (20, 14, 15, 1.0), and calls func_001026A0(D_008105D0, &actor[0xD0], &scratch).
// Returns 1.
extern int func_001026A0(void *, void *, void *);
extern int func_001B6F00(void *, float, void *);
extern float D_00810350;
extern float D_00810354;
extern float D_00810358;
extern float D_008105D0[4];
extern float D_008105E0[4];
extern volatile int D_700038A0;
extern volatile int D_700038A4;
extern volatile int D_700038A8;
extern volatile int D_700038AC;

int func_001575E0(char *arg0) {
    D_700038A0 = 0x41200000;
    D_700038A4 = 0;
    D_700038A8 = 0xC0A00000;
    D_700038AC = 0x3F800000;
    func_001B6F00(arg0, 1.5707964f, &D_700038A0);
    D_008105E0[0] = D_00810350;
    D_008105E0[1] = 11.0f + D_00810354;
    D_008105E0[2] = D_00810358;
    D_008105E0[3] = 1.0f;
    D_700038A0 = 0x41A00000;
    D_700038A4 = 0x41600000;
    D_700038A8 = 0x41700000;
    D_700038AC = 0x3F800000;
    func_001026A0(D_008105D0, arg0 + 0xD0, &D_700038A0);
    return 1;
}
