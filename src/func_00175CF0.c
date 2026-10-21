// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x700031C0
//
// Driven by the entity pointer at 0x700031D0 (e): arg0+0x238 = e[0x1A] &
// 0xFF00 and arg0+0x23B = the low byte of e[0x1A]; func_0019A310(arg0 + 0x9C).
// For byte 0x35 at arg0+0x23B, 0x70003A20 and arg0+0x310 get
// func_001B1470(func_0011E620(-e[0x3C], e[0x34]) - pi/2). Adds the floats at
// 0x700031C0..0x700031C8 to arg0+0xB0..0xB8 and, with arg1 bit 1, copies a
// non-zero word at 0x700031D4 to arg0+0x214. For halfword 0x2000 at arg0+0x238
// (or 0x1000 when func_00175640(arg0+0x214) is non-zero) with the float at
// arg0+0x9C in [0.017453292, 3.1241393]: 0x700031E4 = func_001B1470(pi/2 +
// func_0011E620(-e[0x2C], e[0x24])), arg0+0xB4 -= *0x700031C4, the point
// (0, 0, *0x700031C4 / func_0011E398(arg0+0x9C), 0) at 0x700038A0 goes through
// func_001029C0 / func_00102BB0 / func_001026A0 into 0x700038B0, and its x / z
// are added to arg0+0xB0 / 0xB8. Otherwise arg0+0xA = 1 (arg2 == 0), 3 (arg2 <
// 6) or 2, with bit 0x80 when arg0+0x214 is non-zero; byte 0x39 at arg0+0x23B
// then calls func_0017F9E0 or func_0017FB90 by arg0+4 / arg0+5, and any other
// byte sets arg0+0x237 (1 for halfword 0x1000, which also sets arg0+0x218 the
// way 0x700031E4 is set above; else 0).
// Byte-matched from C with mwcc 2.3.3 (decomp lane round 6, 2026-10-20); it was
// a 99.15 NEARMISS whose header called the filled delay slot a compiler
// artifact. The key is MATCHING_GUIDE idiom-32: 0x700031C0 is a relocated
// extern (opted in with // SPAD:), so mwcc no longer shares one literal upper
// half and leaves the slot as nop, as the target does.

extern float D_700031C0;
extern void func_001026A0(void *a0, void *a1, void *a2);
extern void func_001029C0(void *a0);
extern void func_00102BB0(void *obj, void *obj2, float v);
extern float func_0011E398(float f);
extern float func_0011E620(float y, float x);
extern int func_00175640(char *a0);
extern void func_0017F9E0(char *a0);
extern void func_0017FB90(char *a0, unsigned char a1);
extern void func_0019A310(char *a0);
extern float func_001B1470(float a);
extern int D_700036A0;
extern int D_700038A0;
extern int D_700038B0;

void func_00175CF0(char *arg0, int arg1, int arg2) {
    char *p;
    unsigned char v1;
    unsigned char v1b;
    unsigned short st;
    float t;
    float f0;
    float f1;

    *(unsigned short *)(arg0 + 0x238) = *(unsigned short *)(*(char **)0x700031D0 + 0x1A) & 0xFF00;
    *(unsigned char *)(arg0 + 0x23B) = *(unsigned short *)(*(char **)0x700031D0 + 0x1A);
    func_0019A310(arg0 + 0x9C);

    v1 = *(unsigned char *)(arg0 + 0x23B);
    if (v1 == 0x35) {
        p = *(char **)0x700031D0;
        f0 = func_0011E620(-*(float *)(p + 0x3C), *(float *)(p + 0x34));
        *(float *)0x70003A20 = f0;
        f1 = func_001B1470(f0 - 1.5707964f);
        *(float *)0x70003A20 = f1;
        *(float *)(arg0 + 0x310) = f1;
    }

    *(float *)(arg0 + 0xB0) = *(float *)(arg0 + 0xB0) + D_700031C0;
    *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0xB4) + *(float *)0x700031C4;
    *(float *)(arg0 + 0xB8) = *(float *)(arg0 + 0xB8) + *(float *)0x700031C8;

    if (arg1 & 2) {
        int t1 = *(int *)0x700031D4;
        if (t1 != 0) {
            *(int *)(arg0 + 0x214) = t1;
        }
    }

    st = *(unsigned short *)(arg0 + 0x238);
    if (st == 0x2000 || (st == 0x1000 && func_00175640((char *)(*(int *)(arg0 + 0x214))) != 0)) {
        float ang = *(float *)(arg0 + 0x9C);
        if (!(ang < 0.017453292f) && ang <= 3.1241393f) {
            p = *(char **)0x700031D0;
            *(float *)0x70003A20 = func_0011E620(-*(float *)(p + 0x2C), *(float *)(p + 0x24));
            *(float *)0x700031E4 = func_001B1470(1.5707964f + *(float *)0x70003A20);
            *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0xB4) - *(float *)0x700031C4;
            t = func_0011E398(*(float *)(arg0 + 0x9C));
            *(volatile int *)0x700038A0 = 0;
            *(volatile int *)0x700038A4 = 0;
            *(volatile float *)0x700038A8 = *(float *)0x700031C4 / t;
            *(volatile int *)0x700038AC = 0;
            func_001029C0(&D_700036A0);
            func_00102BB0(&D_700036A0, &D_700036A0, *(float *)0x700031E4);
            func_001026A0(&D_700038B0, &D_700036A0, &D_700038A0);
            *(float *)(arg0 + 0xB0) = *(float *)(arg0 + 0xB0) + *(volatile float *)0x700038B0;
            *(float *)(arg0 + 0xB8) = *(float *)(arg0 + 0xB8) + *(volatile float *)0x700038B8;
        }
    } else {
        if (arg2 == 0) {
            *(unsigned char *)(arg0 + 0xA) = 1;
        } else if (arg2 < 6) {
            *(unsigned char *)(arg0 + 0xA) = 3;
        } else {
            *(unsigned char *)(arg0 + 0xA) = 2;
        }
        if (*(int *)(arg0 + 0x214) != 0) {
            *(unsigned char *)(arg0 + 0xA) = *(unsigned char *)(arg0 + 0xA) | 0x80;
        }

        if (*(unsigned char *)(arg0 + 0x23B) == 0x39) {
            if (arg2 < 6 && *(unsigned char *)(arg0 + 4) == 1 && *(unsigned char *)(arg0 + 5) == 1) {
                func_0017F9E0(arg0);
                return;
            }
            v1b = *(unsigned char *)(arg0 + 4);
            if (v1b == 1 && *(unsigned char *)(arg0 + 5) == 8) {
                func_0017FB90(arg0, v1b);
                return;
            }
            if (v1b == 1 && *(unsigned char *)(arg0 + 5) == 0) {
                func_0017FB90(arg0, v1b);
            }
        } else {
            unsigned short st2 = *(unsigned short *)(arg0 + 0x238);
            if (st2 == 0x1000) {
                *(char *)(arg0 + 0x237) = 1;
                p = *(char **)0x700031D0;
                *(float *)0x70003A20 = func_0011E620(-*(float *)(p + 0x2C), *(float *)(p + 0x24));
                *(float *)(arg0 + 0x218) = func_001B1470(1.5707964f + *(float *)0x70003A20);
                return;
            }
            *(char *)(arg0 + 0x237) = 0;
        }
    }
}
