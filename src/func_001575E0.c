// NEARMISS func_001575E0  (vram 0x001575E0, 0xF4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 92.28% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a scheduling difference no source change has fixed yet:
// Remaining differences: the D_008105E8 and D_008105EC stores are scheduled earlier than in the original (the original keeps D_008105E0..EC stores in order, as if they were one 4-float vector; an array spelling reproduces that schedule but relocates against D_008105E0+n).
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x700038A0 0x700038A4 0x700038A8 0x700038AC

// NEARMISS: 92.28% (mwcc 2.3.3), was 78.93%. Fixes read from the original:
//  - func_001B6F00 takes (owner, yaw, point): arg0 passes through in a0,
//    &D_700038A0 in a1 (twin func_001576E0 declares it so).
//  - The scratchpad words 0x700038A0..AC are relocated volatile externs
//    (idiom-32, // SPAD:), which gives the original's per-store constant
//    loads; the constants are stored as int bit patterns, like the twin.
//  - D_008105EC is stored between the 0x700038A4 and 0x700038A8 words.
// D_008105D0 and D_008105E0 are 4-float vectors (func_00157360 fills both);
// the original most likely wrote D_008105E0 as one vector.
//
// Sets up a point block at 0x700038A0 (10, 0, -5, 1.0) and calls
// func_001B6F00(actor, pi/2, &scratch), then writes the vector D_008105E0
// (D_00810350, 11 + D_00810354, D_00810358, 1.0) and a second point block
// (20, 14, 15, 1.0), and calls func_001026A0(&D_008105D0, &actor[0xD0], &scratch).
// Returns 1.
extern int func_001026A0(void *, void *, void *);
extern int func_001B6F00(void *, float, void *);
extern float D_00810350;
extern float D_00810354;
extern float D_00810358;
extern float D_008105D0;
extern float D_008105E0;
extern float D_008105E4;
extern float D_008105E8;
extern int D_008105EC;
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
    D_008105E0 = D_00810350;
    D_008105E4 = 11.0f + D_00810354;
    D_008105E8 = D_00810358;
    D_700038A0 = 0x41A00000;
    D_700038A4 = 0x41600000;
    D_008105EC = 0x3F800000;
    D_700038A8 = 0x41700000;
    D_700038AC = 0x3F800000;
    func_001026A0(&D_008105D0, arg0 + 0xD0, &D_700038A0);
    return 1;
}
