// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// SEMANTICS: enemy "search/turn" AI state machine (arg0 = entity, arg1 = its AI block):
// state 0 arms the search timers and plays clip 3; 1 waits for input flag 0x1000;
// 2 turns toward the target yaw, counting 3 aligned frames before ending the search;
// 3 sweeps the yaw left/right and probes for a clear path (func_001B3250) before
// returning to state 2; the shared tail ticks the 0x24 timer and bails to state 4/0.
extern void anim_clip_init(char *self, int idx, float a, float b);
extern int func_001416D0(char *arg0, char *arg1);
extern int func_00122BB8(void);
extern float func_0011DF78(float x);
extern float func_001B1240(char *p, float b, float c);
extern float func_001B12B0(float a, float b, float c);
extern float func_001B1470(float a);
extern void func_001029C0(void *m);
extern void func_00102BB0(void *a, void *b, float f);
extern void func_001026A0(void *a, void *b, void *c);
extern void func_001028B8(void *a, void *b, void *c);
extern int func_001B3250(char *p, void *v, float r);
extern int func_001FBD50(char *p, int a, int b, float f);
extern float D_00810360;
extern float D_00810368;
extern int D_700036A0[4];
extern int D_700038A0[4];

void func_0013E6D0(char *arg0, char *arg1) {
    unsigned char st;
    signed char m;
    float ang;
    float sign;

    st = *(unsigned char *)(arg0 + 6);
    switch (st) {
    case 0:
        *(unsigned char *)(arg0 + 6) = st + 1;
        *(int *)(arg1 + 0x20) = 0;
        *(int *)(arg1 + 0x24) = 0x1A4;
        *(float *)(arg1 + 0x40) = 1.0f;
        *(unsigned char *)(arg1 + 0x81) = 1;
        *(unsigned char *)(arg1 + 0x82) = 0;
        *(short *)(arg1 + 0x72) = 0;
        *(int *)(arg1 + 0x48) = 0;
        *(int *)(arg1 + 0x50) = 0;
        *(int *)(arg1 + 0x4C) = 0;
        anim_clip_init(arg0, 3, 5.0f, 0.0f);
        break;
    case 1:
        if (*(int *)(arg1 + 0x30) & 0x1000) {
            *(unsigned char *)(arg0 + 6) = st + 1;
            anim_clip_init(arg0, 4, 0.0f, 0.0f);
        }
        break;
    case 2:
        if (func_001416D0(arg0, arg1) != 0) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
            if (*(signed char *)(arg1 + 0x83) == 2 &&
                func_0011DF78(func_001B1470(*(float *)(arg1 + 0x44) - func_001B1470(3.14159274f + *(float *)(arg0 + 0xC4)))) <= 0.3926991f) {
                m = *(signed char *)(arg1 + 0x82) + 1;
                *(signed char *)(arg1 + 0x82) = m;
                if (!(m <= 2)) {
                    *(int *)(arg1 + 0x24) = 0;
                }
            }
        } else {
            if (*(int *)(arg1 + 0x20) != 0) {
                *(int *)(arg1 + 0x20) = *(int *)(arg1 + 0x20) - 1;
            } else {
                *(int *)(arg1 + 0x20) = (func_00122BB8() >> 6) & 0x1F;
                *(float *)(arg1 + 0x44) = func_001B1240(arg0 + 0xB0, D_00810360, D_00810368);
            }
        }
        *(float *)(arg0 + 0xC4) = func_001B12B0(*(float *)(arg1 + 0x44), *(float *)(arg0 + 0xC4), 0.07853982f);
        break;
    case 3:
        if (*(signed char *)(arg1 + 0x83) == 2) {
            ang = func_001B12B0(*(float *)(arg1 + 0x44), *(float *)(arg0 + 0xC4), 0.07853981852531433f);
            *(float *)(arg0 + 0xC4) = ang;
            if (ang == *(float *)(arg1 + 0x44)) {
                *(unsigned char *)(arg0 + 6) = 2;
            }
        } else {
            if (*(signed char *)(arg1 + 0x83) != 0) {
                sign = -0.3926991f;
                *(float *)(arg0 + 0xC4) = func_001B1470(*(float *)(arg0 + 0xC4) + 0.061086528f);
            } else {
                sign = 0.3926991f;
                *(float *)(arg0 + 0xC4) = func_001B1470(*(float *)(arg0 + 0xC4) - 0.061086528f);
            }
            func_001029C0(D_700036A0);
            func_00102BB0(D_700036A0, D_700036A0, *(float *)(arg0 + 0xC4));
            *(int *)0x700038A0 = 0;
            *(int *)0x700038A4 = 0x40400000;
            *(int *)0x700038A8 = 0x41500000;
            *(int *)0x700038AC = 0x3F800000;
            func_001026A0(D_700038A0, D_700036A0, D_700038A0);
            func_001028B8(D_700038A0, D_700038A0, arg0 + 0xB0);
            if (func_001B3250(arg0, D_700038A0, 20.0f) == 0) {
                func_00102BB0(D_700036A0, D_700036A0, sign);
                *(int *)0x700038A0 = 0;
                *(int *)0x700038A4 = 0x40400000;
                *(int *)0x700038A8 = 0x41500000;
                *(int *)0x700038AC = 0x3F800000;
                func_001026A0(D_700038A0, D_700036A0, D_700038A0);
                func_001028B8(D_700038A0, D_700038A0, arg0 + 0xB0);
                if (func_001B3250(arg0, D_700038A0, 20.0f) == 0) {
                    *(unsigned char *)(arg0 + 6) = 2;
                    *(float *)(arg1 + 0x44) = *(float *)(arg0 + 0xC4);
                    *(int *)(arg1 + 0x24) = ((func_00122BB8() >> 0x12) & 0x1F) + 0xF;
                }
            }
        }
        break;
    }

    if (*(unsigned char *)(arg1 + 0x30) == 9) {
        func_001FBD50(arg0, ((func_00122BB8() >> 0x11) % 5) + 0x7F9, 0, 300.0f);
    }
    *(short *)(arg1 + 0x72) = *(short *)(arg1 + 0x72) + 1;
    if (*(int *)(arg1 + 0x24) != 0) {
        *(int *)(arg1 + 0x24) = *(int *)(arg1 + 0x24) - 1;
    }
    if (*(int *)(arg1 + 0x24) == 0 || *(short *)(arg1 + 0x70) == 0 || *(float *)(arg1 + 0x5C) <= 20.0f) {
        *(signed char *)(arg0 + 5) = 4;
        *(unsigned char *)(arg0 + 6) = 0;
        func_001FBD50(arg0, 0x803, 0, 300.0f);
    }
}
