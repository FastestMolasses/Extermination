// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x700038A0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 96.65% NEARMISS).
// Fixes read from the original instructions:
//  - Each "set bit 4 of arg1+0x60" success path is its own read-or-store (the
//    original has a separate load/or/store per call site); the shared
//    unsigned-char `flags` temporary + goto made mwcc merge the stores and mask
//    the value. Written as plain `|= 4` in an if/else-if chain.
//  - The scratchpad vector 0x700038A0 is a relocated extern (idiom-32, opted in
//    with // SPAD:), declared float[4]; as a relocated symbol mwcc no longer
//    fills the two branch delay slots with its upper half, as the original.
//  - The +18.0f and +0.002f adjustments are compound assignments (`+=`), which
//    gives the original's float register order.
//
// (Reading of the code, not checked against a capture: "collision probe" and
// "rising/falling" are interpretations.)
// Collision probe for an actor (arg0 = actor, arg1 = its collision state).
// Clears the state flags at arg1+0x60, probes a forward vector (sign from
// arg1+0x3C) with func_001B32F0, and when falling (arg1+0x38 < -0.1) probes two
// rotated segment pairs with func_001B3390; any hit sets bit 4 and marks
// arg0+0x52 bit 0. When not rising, tries the four D_00244FE0 offsets against a
// downward ray (func_001B3440; a hit zeroes the vertical speed and sets bit 2),
// may switch the actor into sub-state 4 (func_001339E0) on a long drop, and
// tests the head point with func_0019B6C0 (bit 0 of arg1+0x69). When rising,
// casts upward with func_0019AB20; a hit zeroes the speed and sets bit 0.

extern void func_001026A0(void *a0, void *a1, void *a2);
extern void func_001028B8(void *a0, void *a1, void *a2);
extern void func_001029C0(void *a0);
extern void func_00102BB0(void *a0, void *a1, float f);
extern void func_00102C58(void *a0, void *a1, void *a2);
extern void func_001339E0(char *a0, unsigned char *a1);
extern int func_0019AB20(char *arg0, float *arg1, char *arg2, int arg3);
extern int func_0019B6C0(void *a0, void *a1);
extern void func_001B2B10(char *arg0, void *arg1, void *arg2);
extern int func_001B2F70(void *a0, void *a1);
extern int func_001B32F0(float f, int a0, void *a1);
extern int func_001B3390(float f, int a0, void *a1, void *a2);
extern int func_001B3440(float f, int a0, void *a1, void *a2);
extern float D_00244FE0[4][4];
extern char D_700036A0[];
extern float D_700038A0[4];
extern char D_700038B0[];
extern char D_700038C0[];
extern char D_700038D0[];

void func_001328D0(char *arg0, char *arg1) {
    int i;
    float (*p)[4];

    *(unsigned char *)(arg1 + 0x60) = 0;
    D_700038A0[0] = 0.0f;
    D_700038A0[1] = 3.0f;
    D_700038A0[2] = 3.0f;
    D_700038A0[3] = 1.0f;
    if (*(float *)(arg1 + 0x3C) < 0.0f) {
        D_700038A0[2] = -3.0f;
    }
    func_001B2B10(arg0, D_700038A0, D_700038A0);
    func_001028B8(D_700038A0, arg0 + 0xB0, D_700038A0);
    if (func_001B32F0(0.78539816f, (int)arg0, D_700038A0) != 0) {
        *(unsigned char *)(arg1 + 0x60) |= 4;
    } else if (*(float *)(arg1 + 0x38) < -0.1f) {
        D_700038A0[0] = 0.0f;
        D_700038A0[1] = 0.0f;
        *(float *)0x700038B0 = 0.0f;
        *(float *)0x700038B4 = 0.0f;
        D_700038A0[2] = -3.0f;
        *(float *)0x700038C0 = -3.0f;
        *(float *)0x700038C4 = 0.0f;
        *(float *)0x700038C8 = 0.0f;
        D_700038A0[3] = 1.0f;
        *(float *)0x700038CC = 1.0f;
        *(float *)0x700038B8 = 3.0f;
        *(float *)0x700038D0 = 3.0f;
        *(float *)0x700038BC = 1.0f;
        *(float *)0x700038D4 = 0.0f;
        *(float *)0x700038D8 = 0.0f;
        *(float *)0x700038DC = 1.0f;
        func_001029C0(D_700036A0);
        func_00102BB0(D_700036A0, D_700036A0, *(float *)(arg0 + 0xC4));
        func_001026A0(D_700038A0, D_700036A0, D_700038A0);
        func_001026A0(D_700038B0, D_700036A0, D_700038B0);
        func_001026A0(D_700038C0, D_700036A0, D_700038C0);
        func_001026A0(D_700038D0, D_700036A0, D_700038D0);
        func_001028B8(D_700038A0, arg0 + 0xB0, D_700038A0);
        func_001028B8(D_700038B0, arg0 + 0xB0, D_700038B0);
        func_001028B8(D_700038C0, arg0 + 0xB0, D_700038C0);
        func_001028B8(D_700038D0, arg0 + 0xB0, D_700038D0);
        if (func_001B3390(0.78539816f, (int)arg0, D_700038A0, D_700038B0) != 0) {
            *(unsigned char *)(arg1 + 0x60) |= 4;
        } else if (func_001B3390(0.78539816f, (int)arg0, D_700038B0, D_700038A0) != 0) {
            *(unsigned char *)(arg1 + 0x60) |= 4;
        }
        if (*(unsigned char *)(arg1 + 0x60) == 0) {
            if (func_001B3390(0.78539816f, (int)arg0, D_700038C0, D_700038D0) != 0) {
                *(unsigned char *)(arg1 + 0x60) |= 4;
            } else if (func_001B3390(0.78539816f, (int)arg0, D_700038D0, D_700038C0) != 0) {
                *(unsigned char *)(arg1 + 0x60) |= 4;
            }
        }
    }
    if (*(unsigned char *)(arg1 + 0x60) & 4) {
        *(unsigned short *)(arg0 + 0x52) = *(unsigned short *)(arg0 + 0x52) | 1;
    }
    if (*(float *)(arg1 + 0x38) <= 0.0f) {
        p = D_00244FE0;
        i = 0;
        do {
            *(float *)0x700038B0 = p[0][0];
            *(float *)0x700038B4 = p[0][1];
            *(float *)0x700038B8 = p[0][2];
            *(float *)0x700038BC = p[0][3];
            func_001029C0(D_700036A0);
            func_00102C58(D_700036A0, D_700036A0, arg0 + 0xC0);
            func_001026A0(D_700038B0, D_700036A0, D_700038B0);
            func_001028B8(D_700038B0, D_700038B0, arg0 + 0xB0);
            D_700038A0[0] = 0.0f;
            D_700038A0[1] = -15.0f;
            D_700038A0[2] = 0.0f;
            D_700038A0[3] = 1.0f;
            if (func_001B3440(0.78539816f, (int)arg0, D_700038B0, D_700038A0) != 0) {
                *(float *)(arg1 + 0x38) = 0.0f;
                *(unsigned char *)(arg1 + 0x60) = *(unsigned char *)(arg1 + 0x60) | 2;
                break;
            }
            i += 1;
            p += 1;
        } while (i < 4);
        if (!(*(unsigned char *)(arg1 + 0x60) & 2) && *(unsigned char *)(arg0 + 4) == 1) {
            unsigned char sub_state = *(unsigned char *)(arg0 + 5);
            if (sub_state != 3 && sub_state != 4) {
                if (func_001B2F70(arg0 + 0xB0, D_700038A0) == 0) {
                    D_700038A0[0] = *(float *)(arg0 + 0xB4) - 10.0f;
                }
                if (!(*(float *)(arg0 + 0xB4) - D_700038A0[0] < 10.0f) && *(float *)(arg1 + 0x38) < -0.5f) {
                    *(unsigned char *)(arg0 + 5) = 4;
                    *(char *)(arg0 + 6) = 0;
                    func_001339E0(arg0, (unsigned char *)arg1);
                }
            }
        }
        D_700038A0[0] = *(float *)(arg0 + 0xB0);
        D_700038A0[1] = *(float *)(arg0 + 0xB4);
        D_700038A0[2] = *(float *)(arg0 + 0xB8);
        D_700038A0[3] = 1.0f;
        D_700038A0[1] += 18.0f;
        *(unsigned char *)(arg1 + 0x69) = *(unsigned char *)(arg1 + 0x69) & 0xFE;
        if (func_0019B6C0(D_700038A0, arg0 + 0xB0) != 0 && *(unsigned char *)(*(int *)0x700031D0 + 0x1A) == 0x5B) {
            *(unsigned char *)(arg1 + 0x69) = *(unsigned char *)(arg1 + 0x69) | 1;
            *(float *)(arg1 + 0x40) = *(float *)0x700031B4;
        }
    } else {
        D_700038A0[0] = 0.0f;
        D_700038A0[1] = 15.0f;
        D_700038A0[2] = 0.0f;
        D_700038A0[3] = 1.0f;
        func_001028B8(D_700038B0, D_700038A0, arg0 + 0xB0);
        *(float *)0x700038B4 += 0.002f;
        if (func_0019AB20(arg0, (float *)D_700038B0, (char *)D_700038A0, 0x80000007) != 0) {
            *(float *)(arg1 + 0x38) = 0.0f;
            *(unsigned char *)(arg1 + 0x60) = *(unsigned char *)(arg1 + 0x60) | 1;
        }
    }
}
