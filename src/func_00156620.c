// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x70003B68 0x70003B8A 0x700038A4 0x700038B4

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 95.22% NEARMISS).
// Fixes read from the original instructions:
//  - Every case of the state switch ends in `break` (the old C returned from
//    cases 0-2); this restores the unfilled compare-chain slots and the
//    44-byte size difference.
//  - The scratchpad words 0x700038A4 / 0x700038B4 / 0x70003A20 / 0x70003A24 /
//    0x70003B68 / 0x70003B8A are relocated externs (idiom-32), one per address.
//    The state-1 probe adjust is `A4 += 4; B4 -= 4` with no temporaries.
//  - The range test is plain: store the radius 50.0f, square it in place
//    (`*= 50.0f`), then `if (dist2 > r2)` -> func_001B17A0, else mark +1 and
//    func_001B1D20 (the old int-store / float-reload trick is not needed).
//  - Phase 0's 0xC aim seed converts the random value into a float local first.
//  - The 0x2000 test on +0x36 re-casts the field to short (the original
//    sign-extends the cached value before masking).
//
// Per-state entity driver dispatched on the state byte at arg0+4 (switch
// 0/1/2/3); sub-struct base is arg0+0x1F0.
//   state 0: if func_001B0FD0(arg0)==0, arm the entity (short +0x34=1, byte0=1),
//     init two transforms via func_00102948(sub+0x10,arg0+0xB0) /
//     (sub+0x20,arg0+0xC0), then func_001C6380(self).
//   state 1: if short +0x36 set, advance (byte0=2, byte+4=2), build the scratch
//     probe pair at 0x700038A0/0x700038B0 (func_00102948 x2, raise +A4 by 4,
//     drop +B4 by 4), and if func_0019A570(...) hits, spin up the 0x700036A0
//     effect transform (func_001029C0 / func_00102B08(..,PI/2) / func_00102918 /
//     +0x36D4 += 0.2 / func_001F0460(4,..)). Then always: call the vtable fn at
//     arg0+0x4C, seed 0x700038A0 from D_00810350+arg0+0xB0, store the squared
//     distance func_00102738(...) at 0x70003A20, 50*50 at 0x70003A24, and if
//     within range mark byte+1=1 and func_001B1D20(self), else func_001B17A0(self).
//   state 2: nested switch on byte +5 (sub-phase 0..3): phase 0 fires muzzle FX
//     (func_001EFD20 events keyed by byte+3==0x18/0x2A) and seeds the +0x74 aim
//     angle / +0x38 speed / +0x78 pitch from RNG tables D_00246A00/D_00246A10;
//     phase 1 counts down +0x28, on zero commits the aim and either ends (byte+3
//     0x18/0x2A) or re-arms +0x28=8; phase 2 counts down then promotes; phase 2/3
//     (fallthrough) integrate motion (+0xC0 heading, +0xB0/+0xB8 by speed*sin/cos
//     of +0x74, +0xB4 by pitch +0x78 clamped to -4), do ground/ceiling probes
//     (func_0019AD00 / func_0019AB20), and on a hit emit the landing FX
//     (func_001EFD20 0x80000013/0x8000002E, func_001FC580 0x1A0) or, when grounded
//     near D_00810700==0x15, func_001FBD50(self,0xDB,0,800.0f). Always tail-calls
//     func_001C6380 / func_001A2370(self,arg0+0xD0) / func_001B17A0 / vtable+0x4C.
//   state 3: func_001AFC10(arg0).
// func_00102B08's 3rd argument is a float (PI/2 in $f12).

extern int func_001B0FD0(char *);
extern void func_00102948(void *dst, void *src);
extern void func_001C6380(void *self);
extern int func_0019A570(void *a, void *b, int c, int d);
extern void func_001029C0(void *a);
extern void func_00102B08(void *a, void *b, float c);
extern void func_00102918(void *a, void *b, void *c);
extern void func_001F0460(int a, void *b);
extern void func_001028D0(void *a, void *b, void *c);
extern float func_00102738(void *a, void *b);
extern void func_001B17A0(void *self);
extern void func_001B1D20(void *self);
extern int func_00122BB8(void);
extern float func_001B1470(float a);
extern float func_0011E2A8(float a);
extern float func_0011DE90(float a);
extern void func_001EFD20(int a, void *b);
extern void func_001FC580(void *self, int id);
extern int func_0019AD00(void *self, void *b, int id);
extern int func_0019AB20(void *self, void *b, void *c, int id);
extern int func_001B0D80(void *self);
extern void func_001FBD50(void *self, int a, int b, float c);
extern void func_001A2370(void *a, void *b);
extern void func_001AFC10(char *);

extern float D_00246A00;
extern float D_00246A10;
extern int D_00810350;
extern unsigned char D_00810700;

extern int D_70003B68;
extern short D_70003B8A;
extern float D_700038A4;
extern float D_700038B4;
extern float D_70003A20;
extern float D_70003A24;
extern int D_700036A0;
extern int D_700038A0;
extern int D_700038B0;

void func_00156620(char *arg0) {
    char *sub = arg0 + 0x1F0;
    unsigned char st = *(unsigned char *)(arg0 + 4);

    switch (st) {
    case 0:
        if (func_001B0FD0(arg0) == 0) {
            *(short *)(arg0 + 0x34) = 1;
            *(char *)(arg0 + 0) = 1;
            func_00102948(sub + 0x10, arg0 + 0xB0);
            func_00102948(sub + 0x20, arg0 + 0xC0);
            func_001C6380(arg0);
        }
        break;
    case 1:
        if (*(short *)(arg0 + 0x36) != 0) {
            *(char *)(arg0 + 0) = 2;
            *(char *)(arg0 + 4) = 2;
            func_00102948(&D_700038A0, arg0 + 0xB0);
            func_00102948(&D_700038B0, &D_700038A0);
            D_700038A4 += 4.0f;
            D_700038B4 -= 4.0f;
            if (func_0019A570(&D_700038A0, &D_700038B0, 4, 0) != 0) {
                func_001029C0(&D_700036A0);
                func_00102B08(&D_700036A0, &D_700036A0, 1.5707964f);
                func_00102918(&D_700036A0, &D_700036A0, arg0 + 0xB0);
                *(float *)0x700036D4 += 0.2f;
                func_001F0460(4, &D_700036A0);
            }
        }
        (*(void (**)(char *))(arg0 + 0x4C))(arg0);
        func_001028D0(&D_700038A0, &D_00810350, arg0 + 0xB0);
        *(int *)0x700038AC = 0;
        D_70003A20 = func_00102738(&D_700038A0, &D_700038A0);
        D_70003A24 = 50.0f;
        D_70003A24 *= 50.0f;
        if (D_70003A20 > D_70003A24) {
            func_001B17A0(arg0);
        } else {
            *(char *)(arg0 + 1) = 1;
            func_001B1D20(arg0);
        }
        break;
    case 2: {
        unsigned char s5 = *(unsigned char *)(arg0 + 5);
        switch (s5) {
        case 0: {
            unsigned char k;
            *(float *)0x700038A0 = *(float *)(arg0 + 0xB0);
            D_700038A4 = 7.0f + *(float *)(arg0 + 0xB4);
            *(float *)0x700038A8 = *(float *)(arg0 + 0xB8);
            *(int *)0x700038AC = 0x3F800000;
            func_001EFD20(0x80000013, &D_700038A0);
            k = *(unsigned char *)(arg0 + 3);
            if (k == 0x18 || k == 0x2A) {
                func_001EFD20(0x8000001C, &D_700038A0);
                func_001FC580(arg0, 0x1A1);
            } else {
                func_001EFD20(0x8000002E, &D_700038A0);
                func_001FC580(arg0, 0x19F);
            }
            *(short *)(arg0 + 0x28) = 2;
            *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) + 1;
            k = *(unsigned char *)(arg0 + 3);
            if (k == 0xA) {
                *(float *)(sub + 0x74) = func_001B1470((6.2831855f * (float)(func_00122BB8() & 0xF0)) / 256.0f);
            } else if (k == 0xC) {
                {
                float r = (float)(func_00122BB8() & 0x1F);
                *(float *)(sub + 0x74) = func_001B1470(*(float *)(arg0 + 0xC4) + ((3.1415927f * r) / 180.0f));
                }
            }
            *(float *)(arg0 + 0x38) = *(&D_00246A00 + ((func_00122BB8() & 0x300) >> 8));
            *(float *)(sub + 0x78) = *(&D_00246A10 + ((func_00122BB8() & 0x300) >> 8));
            break;
        }
        case 1: {
            short t = *(short *)(arg0 + 0x28) - 1;
            *(short *)(arg0 + 0x28) = t;
            if (t == 0) {
                unsigned char k;
                *(float *)(arg0 + 0xC4) = *(float *)(sub + 0x74);
                k = *(unsigned char *)(arg0 + 3);
                if (k == 0x18 || k == 0x2A) {
                    *(char *)(arg0 + 4) = 3;
                    *(char *)(arg0 + 5) = 0;
                } else {
                    *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) + 1;
                    *(short *)(arg0 + 0x28) = 8;
                }
            }
            break;
        }
        case 2: {
            short t = *(short *)(arg0 + 0x28);
            if (t == 0) {
                *(unsigned char *)(arg0 + 5) = s5 + 1;
                *(char *)(arg0 + 0) = 1;
                *(short *)(arg0 + 0x36) = 0;
                *(short *)(arg0 + 0x34) = 1;
            } else {
                *(short *)(arg0 + 0x28) = t - 1;
            }
        }
        /* fallthrough */
        case 3:
            if (*(float *)(sub + 0x78) < 0.0f) {
                *(float *)(arg0 + 0xC0) = func_001B1470(0.017453292f + *(float *)(arg0 + 0xC0));
            } else {
                *(float *)(arg0 + 0xC0) = func_001B1470(0.008726646f + *(float *)(arg0 + 0xC0));
            }
            *(float *)(arg0 + 0xB0) = *(float *)(arg0 + 0xB0) + (*(float *)(arg0 + 0x38) * func_0011E2A8(*(float *)(sub + 0x74)));
            *(float *)(arg0 + 0xB8) = *(float *)(arg0 + 0xB8) + (*(float *)(arg0 + 0x38) * func_0011DE90(*(float *)(sub + 0x74)));
            *(float *)0x700038A0 = *(float *)(arg0 + 0xB0) + (7.0f * func_0011E2A8(*(float *)(sub + 0x74)));
            D_700038A4 = 7.0f + *(float *)(arg0 + 0xB4);
            *(float *)0x700038A8 = *(float *)(arg0 + 0xB8) + (7.0f * func_0011DE90(*(float *)(sub + 0x74)));
            if (*(float *)(arg0 + 0x38) != 0.0f && func_0019AD00(arg0, &D_700038A0, 0x80000007) != 0) {
                *(int *)(arg0 + 0x38) = 0;
            }
            *(float *)(sub + 0x78) = *(float *)(sub + 0x78) - 0.06f;
            if (*(float *)(sub + 0x78) < -4.0f) {
                *(float *)(sub + 0x78) = -4.0f;
            }
            *(float *)(arg0 + 0xB4) += *(float *)(sub + 0x78);
            if (*(unsigned char *)(arg0 + 5) == 3 && *(short *)(arg0 + 0x36) != 0) {
                if (!((short)*(short *)(arg0 + 0x36) & 0x2000)) {
                    *(float *)0x700038A0 = *(float *)(arg0 + 0xB0);
                    D_700038A4 = 4.0f + *(float *)(arg0 + 0xB4);
                    *(float *)0x700038A8 = *(float *)(arg0 + 0xB8);
                    *(int *)0x700038AC = 0x3F800000;
                    func_001EFD20(0x80000013, &D_700038A0);
                    func_001EFD20(0x8000002E, &D_700038A0);
                    func_001FC580(arg0, 0x1A0);
                    *(short *)(arg0 + 0x36) = 0;
                    *(char *)(arg0 + 4) = 3;
                    *(char *)(arg0 + 5) = 0;
                    *(char *)(arg0 + 0) = 2;
                    break;
                }
                *(short *)(arg0 + 0x36) = 0;
            }
            if (*(float *)(sub + 0x78) < 0.0f) {
                *(float *)0x700038A0 = 0.0f;
                D_700038A4 = -10.0f;
                *(float *)0x700038A8 = 0.0f;
                *(int *)0x700038AC = 0x3F800000;
                if (func_0019AB20(arg0, arg0 + 0xB0, &D_700038A0, 0x80000007) != 0) {
                    *(float *)0x700038A0 = *(float *)(arg0 + 0xB0);
                    D_700038A4 = 4.0f + *(float *)(arg0 + 0xB4);
                    *(float *)0x700038A8 = *(float *)(arg0 + 0xB8);
                    *(int *)0x700038AC = 0x3F800000;
                    func_001EFD20(0x80000013, &D_700038A0);
                    func_001EFD20(0x8000002E, &D_700038A0);
                    func_001FC580(arg0, 0x1A0);
                    *(short *)(arg0 + 0x36) = 0;
                    *(char *)(arg0 + 4) = 3;
                    *(char *)(arg0 + 5) = 0;
                } else if ((((D_70003B68 + D_70003B8A) & 0x3F) || func_001B0D80(arg0) == 0) && D_00810700 == 0x15 && *(float *)(arg0 + 0xB4) < 5.0f) {
                    *(float *)0x700038A0 = *(float *)(arg0 + 0xB0);
                    D_700038A4 = 10.0f;
                    *(float *)0x700038A8 = *(float *)(arg0 + 0xB8);
                    *(int *)0x700038AC = 0x3F800000;
                    func_001EFD20(0x8000005F, &D_700038A0);
                    func_001FBD50(arg0, 0xDB, 0, 800.0f);
                    *(short *)(arg0 + 0x36) = 0;
                    *(char *)(arg0 + 4) = 3;
                    *(char *)(arg0 + 5) = 0;
                }
            }
            break;
        }
        func_001C6380(arg0);
        func_001A2370(arg0, arg0 + 0xD0);
        func_001B17A0(arg0);
        (*(void (**)(char *))(arg0 + 0x4C))(arg0);
        break;
    }
    case 3:
        func_001AFC10(arg0);
        break;
    }
}
