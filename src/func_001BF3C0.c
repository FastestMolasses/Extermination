// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 87.20% NEARMISS).
// Fixes read from the original instructions:
//  * func_001B1020 takes the actor first: (self, D_00275630[mode], -1, 0), per
//    its definition in src/ (the old call dropped self, shifting every argument).
//  * func_001AFC10 takes only self (its definition is one pointer argument).
//  * D_00275630 is a short[4] (8 bytes, up to D_00275638) addressed $gp-relative,
//    so the file needs -sdatathreshold 8 and a sized declaration.
//  * The dispatch is a real `switch (st)` with case 0 falling into case 1 and
//    every exit a `break` (the old goto/if chain mis-laid the default return).
//
// State-machine update for an entity (arg0). Dispatches on the state byte
// *(u8*)(arg0+4): states 2 and 3 hand off to func_001AFC10(arg0); state 1 runs
// the clamp section; state 0 runs the enter-body (which checks the linked
// entity *(arg0+0x20)->state, binds a model via func_001B1020 indexed by
// D_00275630[arg0[0xD]], and decrements the +0xA4 timer by 3.0/6.0 depending on
// arg0[0xD]) before falling into the clamp.
// The clamp section bails if the linked entity reached state 3 (sets own
// state 3), force-completes when it is state 2 with a zero +0x38, then clamps
// the linked entity's +0x38 value by a per-mode cap (6/9/12), writes
// arg0+0xB4 = arg0+0xA4 + clamped, runs func_001C6380 and the +0x4C vtable cb.

extern short D_00275630[4];

void func_001AFC10(char *);
int func_001B1020(char *, int, int, int);
void func_001C6380(char *);

void func_001BF3C0(char *arg0) {
    char *s0;
    int st;
    float v;

    st = *(unsigned char *)(arg0 + 4);
    s0 = *(char **)(arg0 + 0x20);
    switch (st) {
    case 0:
        if (*(unsigned char *)(s0 + 4) == 3) {
            *(unsigned char *)(arg0 + 4) = 3;
            break;
        }
        func_001B1020(arg0, D_00275630[*(unsigned char *)(arg0 + 0xD)], -1, 0);
        if (*(unsigned char *)(arg0 + 0xD) == 1) {
            *(float *)(arg0 + 0xA4) = *(float *)(arg0 + 0xA4) - 3.0f;
        } else if (*(unsigned char *)(arg0 + 0xD) == 2) {
            *(float *)(arg0 + 0xA4) = *(float *)(arg0 + 0xA4) - 6.0f;
        }
    case 1:
        if (*(unsigned char *)(s0 + 4) == 3) {
            *(unsigned char *)(arg0 + 4) = 3;
            break;
        }
        if (*(unsigned char *)(s0 + 4) == 2 && *(float *)(s0 + 0x38) == 0.0f) {
            *(unsigned char *)(arg0 + 4) = 3;
        }
        v = *(float *)(s0 + 0x38);
        switch (*(unsigned char *)(arg0 + 0xD)) {
        case 0:
            if (v > 6.0f) v = 6.0f;
            break;
        case 1:
            if (v > 9.0f) v = 9.0f;
            break;
        case 2:
            if (v > 12.0f) v = 12.0f;
            break;
        }
        *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0xA4) + v;
        func_001C6380(arg0);
        (*(void (**)(char *))(arg0 + 0x4C))(arg0);
        break;
    case 2:
    case 3:
        func_001AFC10(arg0);
        break;
    }
}
