// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern float func_0011DF78(float);
extern void anim_clip_init(unsigned char *, int, float, float);
extern void func_001FBD50(unsigned char *, int, int, float);
extern float func_001B12B0(float, float, float);
extern void func_00102948(void *, void *);
extern float func_001B15D0(void *, void *);

extern int D_00810360;
extern int D_700038A0;
extern int D_700038B0;

void func_00151E50(unsigned char *arg0, unsigned char *arg1) {
    unsigned char state;
    unsigned char *p;

    state = arg0[6];
    switch (state) {
    case 0:
        arg0[6] = (unsigned char)(state + 1);
        arg1[0x53] = 2;
        if (*(float *)(arg0 + 0xB8) > 150.0f) {
            *(float *)(arg1 + 0x2C) = 3.1415927f;
        } else {
            *(float *)(arg1 + 0x2C) = 0.0f;
        }
        *(float *)(arg1 + 0x20) = func_0011DF78(*(float *)(arg0 + 0xC4) - *(float *)(arg1 + 0x2C)) / 50.0f;
        anim_clip_init(arg0, 1, 5.0f, 0.0f);
        p = *(unsigned char **)(arg0 + 0x20);
        if (p != 0) {
            p[5] = 0xA;
        }
        func_001FBD50(arg0, 0x88D, 0, 500.0f);
        break;
    case 1:
        if ((*(short *)(arg0 + 0x2C) & 0x8000) == 0 && *(float *)(arg0 + 0x3C) <= 80.0f) {
            *(float *)(arg0 + 0xC4) = func_001B12B0(*(float *)(arg1 + 0x2C), *(float *)(arg0 + 0xC4), *(float *)(arg1 + 0x20));
            if ((*(int *)(arg1 + 0) & 0x1000) && *(float *)(arg0 + 0xC4) == *(float *)(arg1 + 0x2C)) {
                func_00102948(&D_700038A0, &D_00810360);
                func_00102948(&D_700038B0, arg0 + 0xB0);
                *(int *)0x700038B4 = 0;
                *(int *)0x700038A4 = 0;
                if (*(signed char *)(arg1 + 0x58) == 0 && !(func_001B15D0(&D_700038A0, &D_700038B0) < 100.0f)) {
                    arg0[5] = 2;
                    arg0[6] = 0;
                } else {
                    arg0[5] = 0;
                    arg0[6] = 0;
                    arg1[0x57] = 0;
                }
            }
        }
        break;
    }
}
