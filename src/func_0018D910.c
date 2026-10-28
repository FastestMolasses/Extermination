// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 81.89% NEARMISS).
// Fixes read from the original instructions:
//  - The body was wrong: the original works on the scratchpad results at
//    0x70003A38/0x70003A3C directly (store, then adjust in place), and when the
//    third (room mode 0x12) probe misses it leaves 0x70003A3C unchanged instead
//    of storing arg0+0x14 + 200.
//  - 0x70003A38/0x70003A3C are relocated scratchpad externs (idiom-32); as
//    literal addresses mwcc copied their address setup into three branch delay
//    slots that the original leaves empty.
//  - The probe vectors at 0x700038A0/0x700038B0/0x700038F0 are float[4]
//    externs indexed directly; the plain element copies give the original's
//    load order.
//
// Camera/look-ahead target height solver. arg0 = camera state, arg1 = target
// entity, arg2 = collision mask. Builds a probe point at D_700038F0: from
// arg1+0xA0 with its Y raised by 11.0, blended (func_001028D0 x2 bracketing a
// func_00102760 normalize) toward arg0+0x10. Casts that probe against
// D_700038A0 (Y-200) via func_0019A910; if it hits AND the hit object's flags
// (+0x1A) have a bit of 0x7000 set, 0x70003A38 = hit distance (0x700031B4)
// +6.0 if arg0+0x5C == 1.0, else +17.0; otherwise 0x70003A38 = arg0+0x50 -
// 200.0. Casts a second probe (D_700038B0, Y+200) the same way; if it hits
// AND the hit flags have a bit of 0x8800 set, 0x70003A3C = hit distance - 1.0.
// Otherwise, if the room mode (D_00810700) is 0x12, casts a third probe from
// arg1+0xB0 to (arg1+0xB0, +0xB4 +200, +0xB8); on a hit, measures the
// func_00102738 distance between (0,-1,0) and the current entity's
// (*0x700031D0) +0x24/0x28/0x2C point into 0x70003A3C and then sets
// 0x70003A3C = hit distance - 1.0 if that distance > 0.2, else arg0+0x14 +
// 200.0; on a miss 0x70003A3C is left as it was. If the room mode isn't 0x12,
// 0x70003A3C = arg0+0x14 + 200.0. Finally clamps 0x70003A38 to at most
// 0x70003A3C - 3.0 and writes the pair back to arg0+0x50/+0x54.
extern void func_00102948(void *dst, void *src);
extern void func_001028D0(void *dst, void *a, void *b);
extern void func_00102760(void *dst, void *src);
extern int func_0019A910(void *a, void *b, int mask);
extern float func_00102738(void *a, void *b);

extern unsigned char D_00810700;
extern float D_700038A0[4];
extern float D_700038B0[4];
extern float D_700038F0[4];
extern float D_70003A38;
extern float D_70003A3C;

void func_0018D910(char *arg0, char *arg1, int arg2) {
    char *cur;

    func_00102948(D_700038F0, arg1 + 0xA0);
    D_700038F0[1] += 11.0f;
    func_001028D0(D_700038F0, arg0 + 0x10, D_700038F0);
    func_00102760(D_700038F0, D_700038F0);
    func_001028D0(D_700038F0, arg0 + 0x10, D_700038F0);

    D_700038A0[0] = D_700038F0[0];
    D_700038A0[1] = D_700038F0[1] - 200.0f;
    D_700038A0[2] = D_700038F0[2];
    D_700038A0[3] = 1.0f;

    if (func_0019A910(D_700038F0, D_700038A0, arg2) != 0 &&
        (*(short *)(*(char **)0x700031D0 + 0x1A) & 0x7000) != 0) {
        D_70003A38 = *(float *)0x700031B4;
        if (1.0f == *(float *)(arg0 + 0x5C)) {
            D_70003A38 += 6.0f;
        } else {
            D_70003A38 += 17.0f;
        }
    } else {
        D_70003A38 = *(float *)(arg0 + 0x50) - 200.0f;
    }

    D_700038B0[0] = D_700038F0[0];
    D_700038B0[1] = 200.0f + D_700038F0[1];
    D_700038B0[2] = D_700038F0[2];
    D_700038B0[3] = 1.0f;

    if (func_0019A910(D_700038F0, D_700038B0, arg2) != 0 &&
        (*(short *)(*(char **)0x700031D0 + 0x1A) & 0x8800) != 0) {
        D_70003A3C = *(float *)0x700031B4;
        D_70003A3C -= 1.0f;
    } else if (D_00810700 == 0x12) {
        D_700038B0[0] = *(float *)(arg1 + 0xB0);
        D_700038B0[1] = 200.0f + *(float *)(arg1 + 0xB4);
        D_700038B0[2] = *(float *)(arg1 + 0xB8);
        D_700038B0[3] = 1.0f;
        if (func_0019A910(arg1 + 0xB0, D_700038B0, arg2) != 0) {
            D_700038B0[0] = 0.0f;
            D_700038B0[1] = -1.0f;
            D_700038B0[2] = 0.0f;
            D_700038B0[3] = 1.0f;
            cur = *(char **)0x700031D0;
            D_700038F0[0] = *(float *)(cur + 0x24);
            D_700038F0[1] = *(float *)(cur + 0x28);
            D_700038F0[2] = *(float *)(cur + 0x2C);
            D_700038F0[3] = 1.0f;
            D_70003A3C = func_00102738(D_700038B0, D_700038F0);
            if (D_70003A3C > 0.2f) {
                D_70003A3C = *(float *)0x700031B4;
                D_70003A3C -= 1.0f;
            } else {
                D_70003A3C = 200.0f + *(float *)(arg0 + 0x14);
            }
        }
    } else {
        D_70003A3C = 200.0f + *(float *)(arg0 + 0x14);
    }

    if (D_70003A38 > D_70003A3C) {
        D_70003A38 = D_70003A3C - 3.0f;
    }
    *(float *)(arg0 + 0x50) = D_70003A38;
    *(float *)(arg0 + 0x54) = D_70003A3C;
}
