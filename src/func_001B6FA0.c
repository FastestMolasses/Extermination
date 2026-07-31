// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Cutscene player-anim/orient compound: a 5-state (arg1[4]) blend dispatcher that faces
// the actor at a target, cross-fades the yaw (arg0+0xC4) and the global camera yaw
// (D_00810374) toward it, drives camera params D_008104A2/A4/A8, kicks anim clips from
// the script record (arg2), publishes the sound/cue block D_002821B0..BC, and returns 1
// only once the abort flag D_008104B0 & 0x1000 is set.

extern void anim_clip_init(char *self, int clip, float a, float b);
extern int func_001B1380(char *a, char *b, float c);
extern float func_001B1240(char *a, float b, float c);
extern float func_001B12B0(float a, float b, float c);
extern float func_001B1470(float a);
extern char D_00810350[];
extern float D_00810360;
extern float D_00810368;
extern volatile float D_00810374;
extern short D_008104A2;
extern int D_008104A4;
extern int D_008104A8;
extern int D_008104B0;
extern int D_002821B0;
extern int D_002821B4;
extern int D_002821B8;
extern int D_002821BC;

int func_001B6FA0(char *arg0, char *arg1, char *arg2)
{
    unsigned char st;
    float f20, f21;

    st = arg1[4];
    switch (st) {
    case 0:
        *(float *)(arg2 + 0x10) = *(float *)(arg0 + 0xC4);
        if (func_001B1380(D_00810350, arg0 + 0xB0, *(float *)(arg0 + 0xC4)) != 0) {
            anim_clip_init(arg0, *(short *)(arg2 + 0x18), 20.0f, 0.0f);
        } else {
            anim_clip_init(arg0, *(short *)(arg2 + 0x14), 20.0f, 0.0f);
        }
        if (func_001B1380(arg0 + 0xB0, D_00810350, D_00810374) != 0) {
            D_008104A2 = 0x156;
        } else {
            D_008104A2 = 0x155;
        }
        D_008104A8 = 0x41A00000;
        D_008104A4 = 0x3F800000;
        arg1[4] = 1;
        break;

    case 1:
        if (*(float *)(arg2 + 0x24) == 0.0f) {
            f20 = *(float *)(arg0 + 0xC4);
        } else {
            f20 = func_001B1240(arg0 + 0xB0, D_00810360, D_00810368);
            *(float *)(arg0 + 0xC4) = func_001B12B0(f20, *(float *)(arg0 + 0xC4), *(float *)(arg2 + 0x24));
        }
        if (*(float *)(arg2 + 0x34) == 0.0f) {
            f21 = D_00810374;
        } else {
            f21 = func_001B1240((char *)&D_00810360, *(float *)(arg0 + 0xB0), *(float *)(arg0 + 0xB8));
            D_00810374 = func_001B12B0(f21, D_00810374, *(float *)(arg2 + 0x34));
        }
        if (D_00810374 == f21) {
            D_008104A2 = 0x164;
            D_008104A8 = 0x41F00000;
        }
        if ((*(float *)(arg0 + 0xC4) == f20) && (D_00810374 == f21)) {
            D_008104A2 = 0x166;
            D_008104A8 = 0x41F00000;
            anim_clip_init(arg0, *(short *)(arg2 + 0x1C), 30.0f, 0.0f);
            D_002821B0 = 2;
            D_002821B4 = 1;
            D_002821B8 = *(int *)(arg2 + 8);
            D_002821BC = 0;
            arg1[4] = 2;
        }
        break;

    case 2:
        if (D_002821B4 != 2) {
            if (D_002821B0 != 0) {
                break;
            }
        }
        D_008104A2 = 0x165;
        D_008104A8 = 0x41F00000;
        if (func_001B1470(*(float *)(arg0 + 0xC4) - *(float *)(arg2 + 0x10)) < 0.0f) {
            anim_clip_init(arg0, *(short *)(arg2 + 0x18), 20.0f, 0.0f);
        } else {
            anim_clip_init(arg0, *(short *)(arg2 + 0x14), 20.0f, 0.0f);
        }
        arg1[4] = 3;
        break;

    case 3:
        if (*(float *)(arg2 + 0x24) == 0.0f) {
            *(float *)(arg0 + 0xC4) = *(float *)(arg2 + 0x10);
        } else {
            *(float *)(arg0 + 0xC4) = func_001B12B0(*(float *)(arg2 + 0x10), *(float *)(arg0 + 0xC4), *(float *)(arg2 + 0x24));
        }
        if (*(float *)(arg0 + 0xC4) == *(float *)(arg2 + 0x10)) {
            anim_clip_init(arg0, *(short *)(arg2 + 4), 20.0f, 0.0f);
            if (D_008104B0 & 0x1000) {
                return 1;
            }
            arg1[4] = 4;
        }
        break;

    case 4:
        if (D_008104B0 & 0x1000) {
            return 1;
        }
        break;
    }
    return 0;
}
