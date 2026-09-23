// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Turret/emplacement AI state machine dispatched on the state byte at
// arg0+4. arg0+0x24 is a pointer to the linked target/player object; arg0+
// 0x1F0 (aliased "s0") is a small per-instance sub-block used as a
// countdown timer (+0x0 int) and an aim/charge-blend pair (+0x54,+0x5C
// floats). State 0 (init): seeds the timer to func_00122BB8()%40+0x3C,
// clears fields +0xC/+9, advances to state 1, and force-advances to state 3
// (disabled) if func_001E23A0(arg0) or bit 3 of func_001B0070() fire.
// State 1 (active): watches the target's flag byte +2 (low 5 bits) and
// either its downed-float +0x220 or its counter byte +4 (>=2 disables to
// state 3), then only while the target's byte +1 is set runs a nested
// sub-state switch on arg0+5: sub-state 0 counts the timer down and, on
// expiry, advances to sub-state 1 and re-seeds the aim-blend pair via
// func_00122BB8(); sub-state 1 ramps the blend float +0x54 by 0.02f per
// tick (clamping/re-arming the timer + resetting to sub-state 0 past 1.5f),
// then fires a shot: builds a look-at matrix from the linked target's
// per-slot transform (func_001026A0/func_001029C0/func_00102C58/
// func_00102918), resolves a spawn handle (func_001CCF70), and launches a
// projectile via func_001CFA60 + func_001CFBE0(handle, 1, &D_00253670,
// &block, 0). States 2/3 both just forward to func_001AFC10(arg0).
//
// MATCH NOTE (m2-matching lane, 98.65% -> 100.0%): (1) the shot's stack
// object is the 0x58-byte projectile block func_001CFA60 fills (it writes
// +0x40..+0x57), so it is int[22] and the frame is 0xA0 as in the target; the
// earlier int[16] was undersized. (2) Every case ends in `break`, not `return`
// (idiom-27): the switch is the last statement, and `return` let mwcc speculate
// the `li 3` into two branch slots the target leaves as nop. (3) The blend ramp
// is `+=` (idiom-26). (4) The slot index is written `idx << 2`, which gives the
// target's `addu v0,v0,s1` operand order; `idx * 4` gave `addu v0,s1,v0`.
// func_001AFC10 takes one argument (its byte-matched definition).
extern int func_001026A0(char *dst, int src, char *m);
extern void func_00102918(char *dst, char *src, char *m);
extern void func_001029C0(char *m);
extern void func_00102C58(char *dst, char *src, char *v);
extern int func_00122BB8(void);
extern void func_001AFC10(char *p);
extern int func_001B0070(void);
extern int func_001CCF70(char *a0);
extern void func_001CFA60(void *obj, char *src, float f12, float f13);
extern void func_001CFBE0(int a0, int a1, void *a2, void *a3, int a4);
extern int func_001E23A0(char *p);
extern int D_00253670;

void func_001E2560(char *arg0) {
    char *s0;
    char *s1;
    unsigned char st;
    unsigned char st2;
    int sp40[22];
    int handle;

    s1 = *(char **)(arg0 + 0x24);
    st = *(unsigned char *)(arg0 + 4);
    s0 = arg0 + 0x1F0;
    switch (st) {
    case 0:
        *(int *)s0 = func_00122BB8() % 40 + 0x3C;
        *(char *)(arg0 + 0xC) = 0;
        *(char *)(arg0 + 9) = 0;
        *(unsigned char *)(arg0 + 4) = 1;
        if (func_001E23A0(arg0) != 0) {
            *(unsigned char *)(arg0 + 4) = 3;
            break;
        }
        if (func_001B0070() & 8) {
            *(unsigned char *)(arg0 + 4) = 3;
            break;
        }
        break;
    case 1:
        if (!(*(unsigned char *)(s1 + 2) & 0x1F)) {
            if (*(float *)(s1 + 0x220) <= 0.0f) {
                *(unsigned char *)(arg0 + 4) = 3;
                break;
            }
        } else if ((int)*(unsigned char *)(s1 + 4) >= 2) {
            *(unsigned char *)(arg0 + 4) = 3;
            break;
        }
        if (*(unsigned char *)(s1 + 1) != 0) {
            st2 = *(unsigned char *)(arg0 + 5);
            switch (st2) {
            case 0:
                *(int *)s0 = *(int *)s0 - 1;
                if (*(int *)s0 < 0) {
                    *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) + 1;
                    *(float *)(s0 + 0x54) = 0.0f;
                    *(float *)(s0 + 0x5C) = (float)func_00122BB8() / 2.1474836e9f;
                    break;
                }
                break;
            case 1:
                *(float *)(s0 + 0x54) += 0.02f;
                if (!(*(float *)(s0 + 0x54) <= 1.5f)) {
                    *(float *)(s0 + 0x54) = 1.5f;
                    *(int *)s0 = func_00122BB8() % 40 + 0x3C;
                    *(unsigned char *)(arg0 + 5) = 0;
                }
                func_001026A0(arg0 + 0xB0, *(int *)((*(int *)(arg0 + 0x28) << 2) + (int)s1 + 0x110) + 0x90, arg0 + 0xA0);
                func_001029C0(arg0 + 0xD0);
                func_00102C58(arg0 + 0xD0, arg0 + 0xD0, s1 + 0xC0);
                func_00102918(arg0 + 0xD0, arg0 + 0xD0, arg0 + 0xB0);
                handle = func_001CCF70(arg0 + 0xB0);
                func_001CFA60(sp40, arg0 + 0xD0, *(float *)(s0 + 0x54), *(float *)(s0 + 0x5C));
                func_001CFBE0(handle, 1, &D_00253670, sp40, 0);
                break;
            }
        }
        break;
    case 2:
    case 3:
        func_001AFC10(arg0);
        break;
    }
}
