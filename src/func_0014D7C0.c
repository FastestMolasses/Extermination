// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
// Per-state animation/clip driver for entity arg0 (state byte at arg0+6) and its companion
// anim record arg1: state 0 seeds the idle timers and starts clip 3 + SFX 0x862; state 1
// starts the per-phase clip from D_00275418[]; state 2 runs a 3-phase proximity gate on
// arg0+0x3C, drives the linked object's byte 5, fires the "player spotted" reaction once,
// and advances/loops the phase; state 3 resets and RNG-picks a facing. All states then tick
// the countdown at arg1+8 and ease arg1+0x28 toward arg1+0x38.
extern void anim_clip_init(char *self, int clip, float a, float b);
extern void func_00102760(void *a, void *b);
extern int func_00122BB8(void);
extern int func_001A7B80(char *p);
extern float func_001B12B0(float goal, float cur, float rate);
extern void func_001B55E0(char *p, int a);
extern void func_001FBD50(char *p, int a, int b, float f);
extern int func_0021BE40(void *a, char *p);

extern short D_00275418[3];
extern volatile unsigned char D_008102B0[16];
extern volatile signed char D_008102BF[16];
extern volatile float D_00810320[16];
extern volatile int D_00810324[16];
extern volatile float D_00810328[16];
extern volatile float D_00810360[16];
extern volatile float D_00810368[16];
extern int D_008104D4[16];
extern unsigned char D_0081070A[16];

void func_0014D7C0(char *arg0, char *arg1) {
    unsigned char st;
    int phase;
    int inrange;
    int zi0;
    float z0;
    int zi1;
    float z1;
    int zi2;
    float z2;
    float lo;
    float v3C;
    char *p;

    st = *(unsigned char *)(arg0 + 6);
    switch (st) {
    case 0:
        *(unsigned char *)(arg0 + 6) = st + 1;
        *(int *)(arg1 + 4) = 0;
        *(int *)(arg1 + 8) = 0x5A;
        *(int *)(arg1 + 0xC) = 0;
        *(float *)(arg1 + 0x20) = 0.525f;
        *(char *)(arg1 + 0x5D) = 0;
        p = *(char **)(arg0 + 0x20);
        if (p != 0) {
            p[5] = 0;
        }
        zi0 = 0;
        z0 = (float)zi0;
        anim_clip_init(arg0, 3, 5.0f, z0);
        func_001FBD50(arg0, 0x862, 0, 300.0f);
        break;
    case 1:
        if (*(int *)(arg1 + 0) & 0x1000) {
            *(unsigned char *)(arg0 + 6) = st + 1;
            zi1 = 0;
            z1 = (float)zi1;
            anim_clip_init(arg0, D_00275418[*(int *)(arg1 + 4)], 0.0f, z1);
            if (*(int *)(arg1 + 4) != 0) {
                *(int *)(arg1 + 8) = 0x2D;
            }
        }
        break;
    case 2:
        phase = *(int *)(arg1 + 4);
        inrange = 0;
        switch (phase) {
        case 0:
            v3C = *(float *)(arg0 + 0x3C);
            lo = 12.0f;
            if (v3C <= 26.0f && !(v3C < lo)) {
                inrange = 1;
            }
            break;
        case 1:
            v3C = *(float *)(arg0 + 0x3C);
            lo = 14.0f;
            if (v3C <= 24.0f && !(v3C < lo)) {
                inrange = 1;
            }
            break;
        case 2:
            v3C = *(float *)(arg0 + 0x3C);
            lo = 22.0f;
            if (v3C <= 30.0f && !(v3C < lo)) {
                inrange = 1;
            }
            break;
        }
        if (inrange) {
            p = *(char **)(arg0 + 0x20);
            if (p != 0) {
                p[5] = 3;
            } else if (p != 0) {
                p[5] = 0;
            }
        }
        if (*(int *)(arg1 + 0xC) == 0 && *(float *)(arg0 + 0x3C) <= lo) {
            *(int *)(arg1 + 0xC) = 1;
            func_001FBD50(arg0, 0x865, 0, 300.0f);
        }
        if (inrange && func_0021BE40((void *)D_008102B0, arg0) == 0 && func_001A7B80(arg0) != 0) {
            D_008102B0[0] |= 2;
            D_008102BF[0] = 2;
            D_00810320[0] = D_00810360[0] - *(float *)(arg0 + 0xB0);
            D_00810324[0] = 0;
            D_00810328[0] = D_00810368[0] - *(float *)(arg0 + 0xB8);
            func_00102760((void *)D_00810320, (void *)D_00810320);
            if (*(int *)(arg1 + 4) == 2) {
                D_008104D4[0] = 0x41C80000;
            } else if (D_0081070A[0] != 0) {
                D_008104D4[0] = 0x41C80000;
            } else {
                D_008104D4[0] = 0x41A00000;
            }
            func_001B55E0(arg0, 1);
        }
        if (*(int *)(arg1 + 0) & 0x1000) {
            phase = *(int *)(arg1 + 4) + 1;
            *(int *)(arg1 + 4) = phase;
            if (phase >= 3) {
                *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
                *(char *)(arg1 + 0x5D) = 1;
                *(float *)(arg1 + 0x20) = 0.2f;
                zi2 = 0;
                z2 = (float)zi2;
                anim_clip_init(arg0, 7, z2, z2);
                if (*(char **)(arg0 + 0x20) != 0) {
                    (*(char **)(arg0 + 0x20))[5] = 0;
                }
            } else {
                *(unsigned char *)(arg0 + 6) = 1;
                *(int *)(arg1 + 0xC) = 0;
                *(float *)(arg1 + 0x20) += 0.15f;
            }
        }
        break;
    case 3:
        if (*(int *)(arg1 + 0) & 0x1000) {
            *(char *)(arg0 + 5) = 0;
            *(unsigned char *)(arg0 + 6) = 0;
            *(float *)(arg1 + 0x20) = 1.0f;
            *(char *)(arg1 + 0x5D) = 0;
            *(short *)(arg1 + 0x50) = (func_00122BB8() >> 0xC) & 0xFF;
        }
        break;
    }

    if (*(int *)(arg1 + 8) != 0) {
        *(int *)(arg1 + 8) = *(int *)(arg1 + 8) - 1;
        *(float *)(arg1 + 0x28) = func_001B12B0(*(float *)(arg1 + 0x38), *(float *)(arg1 + 0x28), 0.0261799395f);
    }
}
