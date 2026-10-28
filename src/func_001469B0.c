// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 93.69% NEARMISS).
// Fixes read from the original instructions and the byte-matched twins
// func_0014BB10 / func_001418F0 (same timer/aim-query shape):
// - the timer-running branch ends in an explicit `return;` (the original
//   jumps to the epilogue from that block rather than sharing the tail);
// - both counter tests are written as `if (c >= limit) { arm/clear }`
//   wrapping the stores, not an early return on `c < limit`; that form lets
//   the timer constant 0x12C be speculated into the preceding branch slots;
// - the owner/entity pointers are plain `char *` as in the twins, and the
//   aim context at arg0+0x14C is read as a byte pointer.
//
// Semantics: timer/state machine on the entity at arg1. If 0x64 (timer)
// is running: while flag 0x7F set or height(0x5C) > 100, increment the
// 0x70 counter and at >= 0xB4 ticks clear timer+counter; otherwise reset
// counter. If timer is 0 and flag 0x7F clear: if arg0+0xA bit0 set, arm
// timer=0x12C; else query func_001B3F10 with an aim angle
// (func_001B1470(atan2(0xB0,0xB8) - PI/2), 18.0) and, when it returns
// nonzero, count up to a table-driven threshold (D_002753F0[D_0081050C&3])
// before arming timer=0x12C; on a zero result reset the counter.

extern float func_0011E620(float, float);
extern float func_001B1470(float);
extern int func_001B3F10(char *, float, float);
extern unsigned char D_002753F0[4];
extern unsigned char D_0081050C[8];

void func_001469B0(char *arg0, char *arg1) {
    unsigned char c;
    char *v0;

    if (*(short *)(arg1 + 0x64) != 0) {
        if (*(char *)(arg1 + 0x7F) == 0 && *(float *)(arg1 + 0x5C) <= 100.0f) {
            *(char *)(arg1 + 0x70) = 0;
        } else {
            c = *(unsigned char *)(arg1 + 0x70) + 1;
            *(unsigned char *)(arg1 + 0x70) = c;
            if (c >= 0xB4) {
                *(short *)(arg1 + 0x64) = 0;
                *(char *)(arg1 + 0x70) = 0;
            }
        }
        return;
    } else {
        if (*(char *)(arg1 + 0x7F) == 0) {
        if (*(unsigned char *)(arg0 + 0xA) & 1) {
            *(short *)(arg1 + 0x64) = 0x12C;
            *(char *)(arg1 + 0x70) = 0;
        } else {
            v0 = *(char **)(arg0 + 0x14C);
            if (func_001B3F10(arg0, func_001B1470(func_0011E620(*(float *)(v0 + 0xB0), *(float *)(v0 + 0xB8)) - 1.5707964f), 18.0f) != 0) {
                c = *(unsigned char *)(arg1 + 0x70) + 1;
                *(unsigned char *)(arg1 + 0x70) = c;
                if (c >= D_002753F0[D_0081050C[0] & 3]) {
                    *(short *)(arg1 + 0x64) = 0x12C;
                    *(char *)(arg1 + 0x70) = 0;
                }
            } else {
                *(char *)(arg1 + 0x70) = 0;
            }
        }
        }
    }
}
