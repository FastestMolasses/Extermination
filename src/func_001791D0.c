// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Two-pass (j<2) per-bone collision/offset routine. Each pass builds two 4-int
// vec params on the stack (different constants per pass), then runs an 8-iter
// inner loop: orient a scratch transform via func_001029C0 / func_001B1470 /
// func_00102BB0, project two points (func_001026A0) into scratch blocks at
// 0x700038B0 and 0x700038C0 offset by the active camera/root at D_00275B40[1],
// and on a func_0019AFE0 hit whose status word masks to 0x2000, set bit (1<<i)
// in p[0x314] and nudge the object position by the contact delta at 0x700031C0.
// Returns 0.
//
// Matched with mwcc 2.3.3; the pinned 991202 build only reaches 85.65%.
extern void func_001029C0(void *a);
extern float func_001B1470(float x);
extern void func_00102BB0(void *a, void *b, float t);
extern void func_001026A0(void *dst, void *a, void *b);
extern int func_0019AFE0(char *p, void *a, void *b, int n);
extern float D_00248950[];
extern char **D_00275B40;
extern char D_700036A0[];
extern char D_700038B0[];
extern char D_700038C0[];

int func_001791D0(char *p) {
    int i;
    int j;
    float *q;
    char **base;
    int sp50[4];
    int sp60[4];

    *(unsigned char *)(p + 0x314) = 0;
    for (j = 0; j < 2; j++) {
        if (j == 0) {
            sp50[0] = 0;
            sp60[0] = 0;
            sp60[1] = 0x408051EC;
            sp50[1] = 0x408051EC;
            sp60[2] = 0;
            sp50[2] = 0x40900000;
            sp60[3] = 0;
            sp50[3] = 0;
        } else {
            sp50[0] = 0;
            sp60[0] = 0;
            sp60[1] = 0x41200000;
            sp50[1] = 0x41200000;
            sp60[2] = 0;
            sp50[2] = 0x40900000;
            sp60[3] = 0;
            sp50[3] = 0;
        }
        q = D_00248950;
        for (i = 0; i < 8; i++) {
            func_001029C0(D_700036A0);
            func_00102BB0(D_700036A0, D_700036A0,
                          func_001B1470(*(float *)(p + 0xC4) + *q));
            func_001026A0(D_700038B0, D_700036A0, sp50);
            base = D_00275B40;
            *(float *)0x700038B0 = *(float *)0x700038B0 + *(float *)(base[1] + 0xC0);
            *(float *)0x700038B8 = *(float *)0x700038B8 + *(float *)(base[1] + 0xC8);
            *(float *)0x700038B4 = *(float *)0x700038B4 + *(float *)(p + 0xB4);
            func_001026A0(D_700038C0, D_700036A0, sp60);
            base = D_00275B40;
            *(float *)0x700038C0 = *(float *)0x700038C0 + *(float *)(base[1] + 0xC0);
            *(float *)0x700038C8 = *(float *)0x700038C8 + *(float *)(base[1] + 0xC8);
            *(float *)0x700038C4 = *(float *)0x700038C4 + *(float *)(p + 0xB4);
            if (func_0019AFE0(p, D_700038C0, D_700038B0, 7) != 0) {
                if ((*(short *)(*(char **)0x700031D0 + 0x1A) & 0xFF00) == 0x2000) {
                    int bit = 1 << i;
                    *(unsigned char *)(p + 0x314) = *(unsigned char *)(p + 0x314) | bit;
                    *(float *)(p + 0xB0) = *(float *)(p + 0xB0) + *(float *)0x700031C0;
                    *(float *)(p + 0xB4) = *(float *)(p + 0xB4) + *(float *)0x700031C4;
                    *(float *)(p + 0xB8) = *(float *)(p + 0xB8) + *(float *)0x700031C8;
                }
            }
            q += 1;
        }
    }
    return 0;
}
