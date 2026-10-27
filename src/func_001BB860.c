// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0


// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 57.68% NEARMISS).
// Fixes read from the original instructions:
//  - Both state dispatches are real switch statements (mwcc 2.3.3 emits the
//    compare chain in reverse order, bodies in source order); the old if/goto
//    chain produced the wrong branch shape. Inner sub-state default falls to
//    the shared tail, outer default returns.
//  - func_001AFC10 takes only the actor pointer (its src/ definition has one
//    parameter); the function itself takes one argument.
//  - Stage-mask test spelled as in the byte-matched twin func_001BC960:
//    array externs and an unsigned shift (1U << ...), which fixes the
//    register colouring of the bit test.
//
// Per-frame update for an actor: state 0 starts the encounter (state -> 1);
// state 1 runs a sub-state machine (line-of-sight pick by actor kind, waits,
// advance), then rebuilds the matrix, calls the per-actor callback at +0x4C,
// and flags the actor when the player is within 20 units; states 2/3 hand off
// to the shared teardown/setup dispatcher func_001AFC10.

extern float func_0011E748(float d);
extern void func_001AFC10(char *self);
extern void func_001B1DE0(char *self);
extern void func_001BB520(char *self);
extern int func_001BB560(char *arg0, char *arg1, int arg2);
extern int func_001BB7C0(void);
extern int func_001BB7F0(char *arg0);
extern void func_001BC150(char *self);
extern void func_001C6380(char *self);

extern float D_00810350;
extern float D_00810354;
extern float D_00810358;
extern unsigned char D_00810700[8];
extern unsigned char D_00810841[];

void func_001BB860(char *self) {
    char *ex = self + 0x1F0;

    switch (*(unsigned char *)(self + 4)) {
    case 0:
        // enter: kick off the encounter and advance to state 1
        func_001BB520(self);
        *(unsigned char *)self = 1;
        break;
    case 1:
        switch (*(unsigned char *)(self + 5)) {
        case 0:
            // sub-state 0: pick a target line-of-sight check based on the
            // actor kind at self+3, then advance the sub-state on success
        {
            unsigned char t = *(unsigned char *)(self + 3);
            if (t == 0x16 || t == 0x17 || t == 0x3E) {
                if (D_00810841[D_00810700[0]] & (1U << *(short *)(self + 0x34))) {
                    if (func_001BB560(self, ex, 0) != 0) {
                        *(unsigned char *)(self + 5) = 2;
                    }
                } else if (func_001BB560(self, ex, 1) != 0) {
                    *(unsigned char *)(self + 5) = *(unsigned char *)(self + 5) + 1;
                }
            } else if (func_001BB560(self, ex, 0) != 0) {
                *(unsigned char *)(self + 5) = 2;
            }
        }
            break;
        case 1:
            if (func_001BB7C0() != 0) {
                *(unsigned char *)(self + 0xB) = 0;
                *(unsigned char *)(self + 5) = 0;
            }
            break;
        case 2:
            if (func_001BB7C0() != 0) {
                *(unsigned char *)(self + 5) = *(unsigned char *)(self + 5) + 1;
            }
            break;
        case 3:
            func_001BC150(self);
            *(unsigned char *)(self + 5) = *(unsigned char *)(self + 5) + 1;
            break;
        case 4:
            if (func_001BB7F0(self) != 0) {
                *(unsigned char *)(self + 5) = 0;
            }
            break;
        }
        func_001C6380(self);
        (*(void (**)(char *))(self + 0x4C))(self);
        {
            // squared distance from the player position (D_00810350/54/58) to self
            float dx = D_00810350 - *(float *)(self + 0xB0);
            float dy = D_00810354 - *(float *)(self + 0xB4);
            float dz = D_00810358 - *(float *)(self + 0xB8);
            if (func_0011E748(dx * dx + dy * dy + dz * dz) <= 20.0f) {
                *(unsigned char *)(self + 1) = 1;
                if (*(unsigned char *)(self + 2) & 0x80) {
                    func_001B1DE0(self);
                }
            }
        }
        break;
    case 2:
    case 3:
        // transition-in-progress: hand off to the shared teardown/setup dispatcher
        func_001AFC10(self);
        break;
    }
}
