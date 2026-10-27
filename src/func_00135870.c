// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 97.11% NEARMISS).
// Fixes read from the original instructions:
//  - state 0 plays clip 0xD on the entity itself with blend 5.0
//    (anim_clip_init(e, 0xD, 5.0f, 0.0f)); the old C passed a bogus pointer
//    and swapped clip/blend.
//  - func_001FBD50 takes (entity, cue, 0, volume) like every byte-matched
//    caller; the old 3-argument extern shifted all arguments by one.
//  - the per-frame turn rate is pi/36 (0x3DB2B8C3, 0.0872664675f), not the
//    5-degree sine constant.
//  - the "arrived" clip-0xF call stages its zero frame through an int
//    (structural twin func_00135D00), which fixes the float argument order.
//  - the second scratchpad copy indexes the entity as a float array, and the
//    listener-vector globals are volatile as in the twin func_001352E0 (the
//    original never hoists their loads across the stores).
//  - the "saved-register order" verdict was wrong: it came from the bad
//    calls above, not a compiler wall.

// Entity anim-state machine, states 0-4: turn-init, turn-to-target-1,
// turn-to-target-2 with timeout, arrival dispatch + wall-avoidance turn kick,
// cooldown reset.
extern void anim_clip_init(void *p, int idx, float a, float b);
void func_00102760(float *dst, float *src);
void func_00102948(float *dst, float *src);
extern int func_00122BB8(void);
int func_001A7B80(void *p);
float func_001B1240(void *p, float x, float z);
float func_001B12B0(float a, float b, float c);
float func_001B15D0(float *a, float *b);
int func_001FBD50(void *a0, int a1, int a2, float f12);
int func_0021BE40(unsigned char *p, void *e);

extern unsigned char D_008102B0;
extern volatile float D_00810320;
extern volatile int D_00810324;
extern volatile float D_00810328;
extern volatile float D_00810360;
extern volatile float D_00810368;
extern int D_008104D4;
extern unsigned char D_0081070A;
extern float D_700038A0;
extern float D_700038B0;

void func_00135870(char *e, char *p1) {
    int doTurn = 0;
    unsigned char st = *(unsigned char *)(e + 6);

    switch (st) {
    case 0:
        *(unsigned char *)(e + 6) = st + 1;
        *(char *)(p1 + 0x5D) = 1;
        *(float *)(p1 + 0x2C) = 0.0f;
        *(int *)(p1 + 0x30) = 0;
        anim_clip_init(e, 0xD, 5.0f, 0.0f);
        break;
    case 1:
        doTurn = 1;
        if (*(int *)p1 & 0x1000) {
            *(int *)(p1 + 4) = 0;
            func_00102948(&D_700038A0, &D_00810360);
            func_00102948(&D_700038B0, (float *)e + 0x2C);
            *(int *)0x700038B4 = 0;
            *(int *)0x700038A4 = 0;
            if (func_001B15D0(&D_700038A0, &D_700038B0) <= 50.0f) {
                *(unsigned char *)(e + 6) = 3;
                {
                    int zi = 0;
                    float z = (float)zi;
                    anim_clip_init(e, 0xF, 0.0f, z);
                }
            } else {
                *(unsigned char *)(e + 6) = *(unsigned char *)(e + 6) + 1;
                *(int *)(p1 + 0x30) = 0x3D4CCCCD;
                anim_clip_init(e, 0xE, 0.0f, 0.0f);
            }
        }
        break;
    case 2:
        *(int *)(p1 + 4) = *(int *)(p1 + 4) + 1;
        doTurn = 1;
        if (!(*(float *)(p1 + 0x2C) <= 0.8f)) {
            *(float *)(p1 + 0x2C) = 0.8f;
            *(int *)(p1 + 0x30) = 0;
        }
        func_00102948(&D_700038A0, &D_00810360);
        func_00102948(&D_700038B0, (float *)e + 0x2C);
        *(int *)0x700038B4 = 0;
        *(int *)0x700038A4 = 0;
        if ((func_001B15D0(&D_700038A0, &D_700038B0) <= 50.0f) ||
            (*(int *)(p1 + 4) >= 0x12C) ||
            (*(char *)(p1 + 0x5C) != 0)) {
            *(unsigned char *)(e + 6) = *(unsigned char *)(e + 6) + 1;
            *(int *)(p1 + 4) = 0;
            *(float *)(p1 + 0x2C) = 0.0f;
            *(int *)(p1 + 0x30) = 0;
            anim_clip_init(e, 0xF, 0.0f, 0.0f);
        }
        break;
    case 3:
        if (*(float *)(e + 0x3C) == 37.0f) {
            func_001FBD50(e, 0x84E, 0, 300.0f);
        }
        if (!(*(float *)(e + 0x3C) < 21.0f)) {
            if (!(*(float *)(p1 + 0x2C) <= 0.5f)) {
                *(float *)(p1 + 0x2C) = 0.5f;
                *(int *)(p1 + 0x30) = 0;
            } else {
                *(int *)(p1 + 0x30) = 0x3D4CCCCD;
            }
        } else {
            *(float *)(p1 + 0x2C) = 0.0f;
            *(int *)(p1 + 0x30) = 0;
        }
        if ((*(int *)(p1 + 4) == 0) && (*(float *)(e + 0x3C) <= 30.0f) &&
            (func_0021BE40(&D_008102B0, e) == 0) && (func_001A7B80(e) != 0)) {
            *(int *)(p1 + 4) = 1;
            D_008102B0 |= 2;
            if (D_0081070A != 0) {
                D_008104D4 = 0x420C0000;
            } else {
                D_008104D4 = 0x41F00000;
            }
            D_00810320 = D_00810360 - *(float *)(e + 0xB0);
            D_00810324 = 0;
            D_00810328 = D_00810368 - *(float *)(e + 0xB8);
            func_00102760(&D_00810320, &D_00810320);
        }
        if (*(int *)p1 & 0x1000) {
            *(unsigned char *)(e + 6) = *(unsigned char *)(e + 6) + 1;
            *(float *)(p1 + 0x2C) = 0.0f;
            *(int *)(p1 + 0x30) = 0;
            anim_clip_init(e, 0x10, 0.0f, 0.0f);
        }
        break;
    case 4:
        if (*(int *)p1 & 0x1000) {
            *(char *)(e + 5) = 0;
            *(unsigned char *)(e + 6) = 0;
            *(char *)(p1 + 0x5D) = 0;
            *(short *)(p1 + 0x50) = ((func_00122BB8() >> 0xB) & 0x1F) + 0x3C;
        }
        break;
    }
    if (doTurn != 0) {
        float ang = func_001B1240((float *)(e + 0xB0), D_00810360, D_00810368);
        *(float *)(p1 + 0x24) = ang;
        *(float *)(e + 0xC4) = func_001B12B0(ang, *(float *)(e + 0xC4), 0.0872664675f);
    }
}
