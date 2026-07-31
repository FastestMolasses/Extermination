// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Four-state blend/transition tick for actor `p` driving anim-context `e` (state byte p[6]):
// 0 kicks clip 3 and clears the blend timer; 1 eases the actor's heading toward the value
// returned by func_001B1240 and, once flag 0x1000 is set on e+0x20, advances to clip 4;
// 2 clamps the blend value to 0.6, stages a probe volume in the scratchpad at 0x700038A0,
// runs the three collision probes and advances to clip 5 on any hit; 3 floors the blend
// value at 0 and, on flag 0x1000, resets both state bytes and restarts clip 0.
// The empty-bodied `else` in the case-2 clamp is load-bearing: the self-store is dead-code
// eliminated (zero instructions emitted), but the extra CFG edge stops mwcc speculating the
// following scratchpad store's `lui $at, 0x7000` into the `bc1t` delay slot.
extern void anim_clip_init(char *self, int clip, float a, float b);
extern float func_001B1240(char *p, float c, float d);
extern float func_001B12B0(float a, float b, float c);
extern void func_001B2B10(char *p, void *a, void *b);
extern void func_001028B8(void *a, void *b, void *c);
extern int func_001B3250(char *p, void *v, float r);
extern int func_001B1560(char *p, void *v, float r);
extern int func_001B13F0(void *v, char *p, float r);

extern float D_00810360;
extern float D_00810368;
extern char D_700038A0[];

void func_00148130(unsigned char *p, unsigned char *e) {
    unsigned char st;

    st = p[6];
    switch (st) {
    case 0:
        p[6] = p[6] + 1;
        *(int *)(e + 0x50) = 0;
        *(float *)(e + 0x4C) = 0.0f;
        anim_clip_init((char *)p, 3, 1.0f, 0.0f);
        break;
    case 1:
        *(float *)(e + 0x44) = func_001B1240((char *)(p + 0xB0), D_00810360, D_00810368);
        *(float *)(p + 0xC4) = func_001B12B0(*(float *)(e + 0x44), *(float *)(p + 0xC4), 0.034906585f);
        if (*(int *)(e + 0x20) & 0x1000) {
            p[6] = p[6] + 1;
            *(int *)(e + 0x50) = 0x3CA3D70A;
            anim_clip_init((char *)p, 4, 1.0f, 0.0f);
        }
        break;
    case 2:
        if (*(float *)(e + 0x4C) > 0.6f) {
            *(float *)(e + 0x4C) = 0.6f;
            *(int *)(e + 0x50) = 0;
        } else {
            *(int *)(e + 0x50) = *(int *)(e + 0x50);
        }
        *(float *)0x700038A0 = 0.0f;
        *(float *)0x700038A4 = 3.0f;
        *(float *)0x700038A8 = 30.0f;
        *(float *)0x700038AC = 1.0f;
        func_001B2B10((char *)p, D_700038A0, D_700038A0);
        func_001028B8(D_700038A0, D_700038A0, p + 0xB0);
        if (func_001B3250((char *)p, D_700038A0, 15.0f) != 0 || (((char *)e)[0x81] & 1)) {
            p[6] = p[6] + 1;
            *(int *)(e + 0x50) = 0xBC23D70A;
            anim_clip_init((char *)p, 5, 1.0f, 0.0f);
            break;
        }
        if (func_001B1560((char *)p, &D_00810360, 1.3962635f) == 0 ||
            func_001B13F0(&D_00810360, (char *)(p + 0xB0), 30.0f) != 0) {
            p[6] = p[6] + 1;
            *(int *)(e + 0x50) = 0xBC23D70A;
            anim_clip_init((char *)p, 5, 1.0f, 0.0f);
        }
        break;
    case 3:
        if (*(float *)(e + 0x4C) < 0.0f) {
            *(int *)(e + 0x50) = 0;
            *(float *)(e + 0x4C) = 0.0f;
        }
        if (*(int *)(e + 0x20) & 0x1000) {
            p[5] = 0;
            p[6] = 0;
            *(int *)(e + 0x50) = 0;
            *(float *)(e + 0x4C) = 0.0f;
            anim_clip_init((char *)p, 0, 5.0f, 0.0f);
        }
        break;
    }
}
