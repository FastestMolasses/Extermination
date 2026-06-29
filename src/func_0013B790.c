// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Per-frame state/anim tick: drives a small state machine on arg0[6], fires anim/sfx triggers, then integrates a clamped velocity field (arg1+0x40/0x4C) into a phase accumulator (arg0+0xB4).
void anim_clip_init(char *, int, float, float);
int func_001B1190(unsigned char, unsigned char);
int func_001EFE00(int, char *);
void func_001FBD50(char *, int, int, float);

void func_0013B790(char *arg0, char *arg1) {
    unsigned char tmp;
    unsigned short cnt;
    float dv;

    tmp = *(unsigned char *)(arg0 + 6);
    switch (tmp) {
    case 0:
        *(unsigned char *)(arg0 + 6) = (unsigned char)(tmp + 1);
        *(unsigned short *)(arg1 + 0x20) = 0x1A4;
        *(int *)(arg1 + 0x50) = 0;
        *(float *)(arg1 + 0x4C) = 0.0f;
        *(float *)(arg1 + 0x40) = -0.02f;
        func_001B1190(*(unsigned char *)(arg0 + 0x9A), tmp);
        func_001FBD50(arg0, 0x81B, 0, 300.0f);
        anim_clip_init(arg0, 0x11, 1.0f, 0.0f);
    case 1:
        if (*(char *)(arg1 + 0x81) & 2) {
            *(unsigned char *)(arg0 + 6) = (unsigned char)(*(unsigned char *)(arg0 + 6) + 1);
            func_001FBD50(arg0, 0x81C, 0, 300.0f);
            func_001FBD50(arg0, 0x81E, 0, 300.0f);
            anim_clip_init(arg0, 0x13, 1.0f, 0.0f);
        }
        cnt = *(unsigned short *)(arg1 + 0x20) - 1;
        *(unsigned short *)(arg1 + 0x20) = cnt;
        if (cnt == 0) {
            *(char *)(arg0 + 4) = 3;
        }
        break;
    case 2:
        if (*(int *)(arg1 + 0x70) & 0x1000) {
            *(unsigned char *)(arg0 + 6) = (unsigned char)(tmp + 1);
            *(char *)(arg1 + 0x87) = 0;
            func_001FBD50(arg0, 0x81D, 0, 300.0f);
            if (func_001EFE00(0x8000001E, arg0) == 0) {
                *(char *)(arg0 + 4) = 3;
            }
        }
        break;
    case 3:
        break;
    }
    if (*(char *)(arg1 + 0x81) & 2) {
        *(float *)(arg1 + 0x40) = 0.0f;
        *(float *)(arg1 + 0x4C) = 0.0f;
    } else if (*(float *)(arg1 + 0x4C) < -3.0f) {
        *(float *)(arg1 + 0x4C) = -3.0f;
        *(float *)(arg1 + 0x40) = 0.0f;
    } else {
        *(float *)(arg1 + 0x40) = -0.02f;
    }
    dv = *(float *)(arg1 + 0x40);
    dv = *(float *)(arg1 + 0x4C) + dv;
    *(float *)(arg1 + 0x4C) = dv;
    *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0xB4) + dv;
}
