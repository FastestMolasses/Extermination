// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Per-state actor/animation driver dispatched on the state byte at arg0+6: state 0 seeds
// the facing angle and starts clip 2, state 1 fires sound 0x88E when close and eases the
// facing toward the goal, state 2 raises the global alert flags / camera look-at and
// starts clip 4, state 3 clears the state on the arg1 bit-0x1000 handoff.

extern void anim_clip_init(char *p, int clip, float speed, float d);
extern void func_001026A0(void *, void *, void *);
extern void func_001029C0(void *);
extern void func_00102BB0(void *, void *, float);
extern int func_001A7B80(char *);
extern float func_001B1240(char *, float, float);
extern float func_001B12B0(float, float, float);
extern int func_001B55E0(char *, int);
extern void func_001FBD50(char *, int, int, float);
extern int func_0021BE40(char *, char *);

extern volatile unsigned char D_008102B0;
extern volatile char D_008102BF;
extern float D_00810320;
extern int D_00810324;
extern float D_00810328;
extern float D_0081032C;
extern float D_00810360;
extern float D_00810368;
extern float D_008104D4;
extern unsigned char D_0081070A;
extern char D_700036A0[];

void func_00152040(char *arg0, char *arg1) {
    unsigned char st;
    char *p;

    st = *(unsigned char *)(arg0 + 6);
    switch (st) {
    case 0:
        *(unsigned char *)(arg0 + 6) = st + 1;
        *(int *)(arg1 + 4) = 0;
        *(char *)(arg1 + 0x53) = 1;
        if (*(float *)(arg0 + 0xB8) < 150.0f) {
            *(float *)(arg1 + 0x2C) = 0.0f;
        } else {
            *(float *)(arg1 + 0x2C) = 3.1415927f;
        }
        anim_clip_init(arg0, 2, 5.0f, 0.0f);
        p = *(char **)(arg0 + 0x20);
        if (p != 0) {
            *(char *)(p + 5) = 0xA;
        }
        break;
    case 1:
        if (!(*(short *)(arg0 + 0x2C) & 0x8000) && *(int *)(arg1 + 4) == 0 &&
            *(float *)(arg0 + 0x3C) <= 52.0f) {
            *(int *)(arg1 + 4) = 1;
            func_001FBD50(arg0, 0x88E, 0, 500.0f);
        }
        *(float *)(arg0 + 0xC4) =
            func_001B12B0(*(float *)(arg1 + 0x2C), *(float *)(arg0 + 0xC4), 0.06981317f);
        if ((*(int *)(arg1 + 0) & 0x1000) &&
            *(float *)(arg0 + 0xC4) == *(float *)(arg1 + 0x2C)) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
            anim_clip_init(arg0, 3, 0.0f, 0.0f);
        }
        break;
    case 2:
        if (func_0021BE40((char *)&D_008102B0, arg0) == 0 && func_001A7B80(arg0) != 0) {
            D_008102B0 |= 2;
            D_008102BF = 2;
            if (D_0081070A != 0) {
                D_008104D4 = 25.0f;
            } else {
                D_008104D4 = 20.0f;
            }
            *(float *)0x70003A20 = func_001B1240(arg0 + 0xB0, D_00810360, D_00810368);
            func_001029C0(&D_700036A0);
            func_00102BB0(&D_700036A0, &D_700036A0, *(float *)0x70003A20);
            D_00810320 = 0.0f;
            D_00810324 = 0;
            D_00810328 = 1.0f;
            D_0081032C = 1.0f;
            func_001026A0(&D_00810320, &D_700036A0, &D_00810320);
            func_001B55E0(arg0, 1);
        }
        if (*(int *)(arg1 + 0) & 0x1000) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
            anim_clip_init(arg0, 4, 0.0f, 0.0f);
        }
        break;
    case 3:
        if (*(int *)(arg1 + 0) & 0x1000) {
            *(char *)(arg0 + 5) = 0;
            *(char *)(arg0 + 6) = 0;
        }
        break;
    }
}
