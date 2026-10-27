// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x700038A0 0x700038A4 0x700038A8 0x700038AC 0x700038B0 0x700038B4 0x700038B8 0x700038BC
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 91.45% NEARMISS).
// Fixes read from the original instructions:
//  - The two work vectors are relocated scratchpad externs (idiom-32, opted in
//    with // SPAD:), not literal-address macros. The A vector (0x700038A0..AC)
//    is volatile; the B vector (0x700038B0..BC) is a plain extern. That split
//    is what lets mwcc hoist the D_00810350/54/58 loads above the A stores
//    (no aliasing with a plain global) while the arg0 loads stay in order, and
//    it reloads both y words before the two += 10.0f stores, as the original.
//  - The seeding order is A.xyz from arg0+0xB0, B.xyz from D_00810350, then
//    both w words = 1.0f, then both y words += 10.0f (the old body interleaved
//    them).
//  - The func_001B15D0 distance stored at 0x70003A20 is the relocated volatile
//    extern D_70003A20, so its upper half is no longer speculated into a
//    branch delay slot.
//
// Proximity/attack-start check: arg0 = self, arg1 = owner. Bails out when the
// owner is busy (+0x5A / +0x61), self is in state 3 or 4, or func_0021BE40 says
// the player is unavailable. Seeds self and player positions (each raised by
// 10.0f) into the scratchpad vectors, requires a clear line (func_0019A570), a
// small height gap unless self flag 0xD&2, a planar distance <= 50 and a facing
// cone (func_001B1560, 45 degrees). Within 24 units it picks an action
// (state 7 or 5) from the random tables D_00245120; farther away it picks
// state 6 or 5 from D_00245130. Returns 1 when an action was started.

extern int func_0021BE40(float *p, char *a);
extern int func_0019A570(float *a, float *b, int c, int d);
extern float func_0011DF78(float d);
extern float func_001B15D0(float *a, float *b);
extern int func_001B1560(char *p, float *q, float ang);
extern int func_0021BED0(float *p);
extern int func_00122BB8(void);

extern float D_00810350;
extern float D_00810354;
extern float D_00810358;
extern float D_00810360;
extern char D_008102B0;
extern unsigned char D_00245120[];
extern unsigned char D_00245130[];

extern volatile float D_700038A0;
extern volatile float D_700038A4;
extern volatile float D_700038A8;
extern volatile int D_700038AC;
extern float D_700038B0;
extern float D_700038B4;
extern float D_700038B8;
extern int D_700038BC;
extern volatile float D_70003A20;

int func_00133640(char *a0, char *a1) {
    float dist;

    if (*(unsigned short *)(a1 + 0x5A) != 0) {
        return 0;
    }
    if (*(unsigned char *)(a1 + 0x61) != 0) {
        return 0;
    }
    if (*(short *)(a0 + 0x2C) == 3 || *(short *)(a0 + 0x2C) == 4) {
        return 0;
    }
    if (func_0021BE40((float *)&D_008102B0, a0) != 0) {
        return 0;
    }
    D_700038A0 = *(float *)(a0 + 0xB0);
    D_700038A4 = *(float *)(a0 + 0xB4);
    D_700038A8 = *(float *)(a0 + 0xB8);
    D_700038B0 = D_00810350;
    D_700038B4 = D_00810354;
    D_700038B8 = D_00810358;
    D_700038AC = 0x3F800000;
    D_700038BC = 0x3F800000;
    D_700038A4 += 10.0f;
    D_700038B4 += 10.0f;
    if (func_0019A570((float *)&D_700038A0, (float *)&D_700038B0, 6, 0) != 0) {
        return 0;
    }
    dist = func_0011DF78(D_700038A4 - D_700038B4);
    if (!(*(unsigned char *)(a0 + 0xD) & 2) && !(dist < 13.0f)) {
        return 0;
    }
    D_700038B4 = 0.0f;
    D_700038A4 = 0.0f;
    D_70003A20 = func_001B15D0((float *)&D_700038A0, (float *)&D_700038B0);
    if (!(D_70003A20 <= 50.0f)) {
        return 0;
    }
    if (func_001B1560(a0, &D_00810360, 0.7853982f) == 0) {
        return 0;
    }
    if (D_70003A20 <= 24.0f) {
        if (D_00245120[(func_00122BB8() >> 4) & 0xF] != 0) {
            if (!(*(unsigned char *)(a0 + 0xD) & 2) && func_0021BED0((float *)&D_008102B0) == 0) {
                *(unsigned char *)(a1 + 0x63) = 0;
                *(unsigned char *)(a0 + 5) = 7;
                *(unsigned char *)(a0 + 6) = 0;
                return 1;
            }
        }
        if (dist < 13.0f) {
            *(unsigned char *)(a1 + 0x64) = (func_00122BB8() >> 0xB) & 1;
            *(unsigned char *)(a1 + 0x63) = 0;
            *(unsigned char *)(a0 + 5) = 5;
            *(unsigned char *)(a0 + 6) = 0;
            return 1;
        }
        *(unsigned char *)(a1 + 0x63) = 0;
        *(unsigned char *)(a0 + 5) = 5;
        *(unsigned char *)(a0 + 6) = 0;
        *(unsigned char *)(a1 + 0x64) = 2;
        return 1;
    }

    {
        unsigned char sel = D_00245130[(func_00122BB8() >> 0x11) & 0xF];
        if (sel == 0) {
            return 0;
        }
        if (sel == 2) {
            if (*(unsigned char *)(a0 + 0xD) & 1) {
                *(unsigned char *)(a1 + 0x63) = 0;
                *(unsigned char *)(a0 + 5) = 6;
                *(unsigned char *)(a0 + 6) = 0;
                return 1;
            }
        }
        *(unsigned char *)(a1 + 0x63) = 0;
        *(unsigned char *)(a0 + 5) = 5;
        *(unsigned char *)(a0 + 6) = 0;
        *(unsigned char *)(a1 + 0x64) = 2;
        return 1;
    }
}
