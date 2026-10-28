// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 76.06% NEARMISS).
// Fixes read from the original instructions:
//  - The loop counter is declared first (it shares the arg0 saved register).
//  - Each random value is the SIGNED seed shifted right 16 and masked to
//    16 bits (the old C used an unsigned shift without the mask); every value
//    is read before the seed advances (seed = seed * 37 + 11).
//  - Each spread value is spelled as the matched AREA00 twins do:
//    r = (float)value; r = r / 65535.0f; r += 0.0001f; then stored as
//    r - 0.5f and 2.4f * r - 1.2f.
//  - 0x70003A20 is the relocated extern D_70003A20; the 0x700038A0 word block
//    is a relocated int/float word array for the +4 clears and the angle store,
//    so mwcc keeps the scratchpad upper half out of the branch delay slots.
//  - The state switch ends its cases in break.
//
// Spawn a 12-particle spray from the actor's work block (self+0x1F0). State
// (+0x04): 0 INIT (copy the 4x4 matrix from +0x24, keep that pointer at +0x40,
// seed +0x44 from func_00122BB8, go to 1, fall through), 1 RUN (distance from
// the work matrix origin to the source matrix origin; past 7 clamp to 7 and go
// to state 3; an intensity word of 0x80 fading to 0 between distance 4 and 7; 12 particles,
// each with an angle mod 128, an index mod 48 and two spread offsets from four
// LCG draws, transformed through both matrices and submitted with
// func_001CD940), 2/3 -> func_001AFC10 teardown.
typedef union { float f; int i; } SpWord;

extern void copy_qw4(void *dst, void *src);
extern int float_to_int(float a);
extern void func_001026A0(void *dst, void *matrix, void *vec);
extern void func_001028D0(void *dst, void *a, void *b);
extern float func_0011E748(float a);
extern int func_00122BB8(void);
extern void func_001AFC10(void *a);
extern void func_001CD940(int mode, void *a, void *b, void *c, void *d);

extern float D_70003A20;
extern SpWord D_700038A0[4];
extern char D_700038B0[];
extern char D_700038C0[];
extern char D_700038D0[];

void func_001F1AA0(char *arg0) {
    int i;
    char *self;
    int seed;
    int hi;
    float d2;
    float r;

    self = arg0 + 0x1F0;
    switch (*(unsigned char *)(arg0 + 4)) {
    case 0:
        copy_qw4(self, *(char **)(arg0 + 0x24));
        *(char **)(self + 0x40) = *(char **)(arg0 + 0x24);
        *(int *)(self + 0x44) = func_00122BB8();
        *(unsigned char *)(arg0 + 4) = 1;
    case 1:
        func_001028D0(D_700038A0, self + 0x30, *(char **)(self + 0x40) + 0x30);
        D_70003A20 = func_0011E748(*(float *)0x700038A0 * *(float *)0x700038A0
                                           + *(float *)0x700038A4 * *(float *)0x700038A4
                                           + *(float *)0x700038A8 * *(float *)0x700038A8);
        if (!(D_70003A20 <= 7.0f)) {
            D_70003A20 = 7.0f;
            *(unsigned char *)(arg0 + 4) = 3;
        }
        d2 = D_70003A20;
        if (!(d2 <= 4.0f)) {
            D_700038A0[1].f = 0.0f;
            *(int *)0x700038AC = float_to_int(128.0f * (1.0f - (d2 - 4.0f) / 3.0f));
        } else {
            D_700038A0[1].f = 0.0f;
            *(int *)0x700038AC = 0x80;
        }
        *(int *)0x700038B4 = 0;
        *(float *)0x700038BC = 1.0f;
        seed = *(int *)(self + 0x44);
        for (i = 0; i < 12; i++) {
            hi = (seed >> 16) & 0xFFFF;
            seed = seed * 37 + 11;
            D_700038A0[0].i = hi % 128;
            hi = (seed >> 16) & 0xFFFF;
            seed = seed * 37 + 11;
            *(int *)0x700038A8 = hi % 48;
            r = (float)((seed >> 16) & 0xFFFF);
            r = r / 65535.0f;
            r += 0.0001f;
            seed = seed * 37 + 11;
            *(float *)0x700038B0 = r - 0.5f;
            r = (float)((seed >> 16) & 0xFFFF);
            r = r / 65535.0f;
            r += 0.0001f;
            seed = seed * 37 + 11;
            *(float *)0x700038B8 = 2.4f * r - 1.2f;
            func_001026A0(D_700038C0, self, D_700038B0);
            func_001026A0(D_700038D0, *(char **)(self + 0x40), D_700038B0);
            func_001CD940(1, D_700038C0, D_700038A0, D_700038D0, D_700038A0);
        }
        break;
    case 2:
    case 3:
        func_001AFC10(arg0);
        break;
    }
}
