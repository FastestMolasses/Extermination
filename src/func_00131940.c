// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Per-state init driver dispatched on the state byte at arg0+6.
// State 0: advance state, set up flags/floats, compute an angle via
// func_001B1470(func_0011E620(arg0+0x70, arg0+0x78) - arg0+0xC4), branch on
// func_0011DF78(angle) < PI/2 to pick +/-1.0f facing and the 0x25/0x26 clip.
// State 1: when arg1+0x60 has bit 2, re-arm timer, clear arg1+0x3C, and when
// arg1+0x58 has bit 0x1000 either fully re-init (arg0+0x34 set) or just mark.

extern void func_001FBD50(char *self, int a, int b, float f);
extern float func_0011E620(float a, float b);
extern float func_001B1470(float a);
extern float func_0011DF78(float a);
extern void anim_clip_init(char *self, int clip, float a, float b);

void func_00131940(char *arg0, char *arg1) {
    float ang;

    switch (*(unsigned char *)(arg0 + 6)) {
    case 0:
        *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
        *(char *)(arg1 + 0x63) = 0;
        *(int *)(arg1 + 0x34) = 0x3F800000;
        *(int *)(arg1 + 0x38) = 0x3FC00000;
        *(unsigned short *)(arg1 + 0x54) = 1;
        func_001FBD50(arg0, 0x7D4, 0, 300.0f);
        ang = func_001B1470(func_0011E620(*(float *)(arg0 + 0x70), *(float *)(arg0 + 0x78)) - *(float *)(arg0 + 0xC4));
        if (!(func_0011DF78(ang) < 1.5707964f)) {
            *(int *)(arg1 + 0x3C) = 0xBF800000;
            *(float *)(arg0 + 0xC4) = func_001B1470(3.1415927f + (*(float *)(arg0 + 0xC4) + ang));
            anim_clip_init(arg0, 0x25, 0.0f, 0.0f);
        } else {
            *(int *)(arg1 + 0x3C) = 0x3F800000;
            *(float *)(arg0 + 0xC4) = func_001B1470(*(float *)(arg0 + 0xC4) + ang);
            anim_clip_init(arg0, 0x26, 0.0f, 0.0f);
        }
        break;
    case 1:
        if (*(unsigned char *)(arg1 + 0x60) & 2) {
            if (*(unsigned short *)(arg1 + 0x54) != 0) {
                *(unsigned short *)(arg1 + 0x54) = 0;
                func_001FBD50(arg0, 0x7D5, 0, 300.0f);
            }
            *(int *)(arg1 + 0x3C) = 0;
            if (*(unsigned short *)(arg1 + 0x58) & 0x1000) {
                if (*(short *)(arg0 + 0x34) != 0) {
                    *(char *)(arg1 + 0x61) = 1;
                    *(char *)(arg0 + 4) = 1;
                    *(char *)(arg0 + 5) = 0;
                    *(unsigned char *)(arg0 + 6) = 0;
                    *(short *)(arg0 + 0x36) = 0;
                    *(char *)(arg0 + 0) = 1;
                    *(char *)(arg1 + 0x6A) = 0x3C;
                } else {
                    *(char *)(arg0 + 5) = 2;
                    *(unsigned char *)(arg0 + 6) = 0;
                }
            }
        }
        break;
    }
}
