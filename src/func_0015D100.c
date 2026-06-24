// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Per-frame timer/decay for an entity's "idle" animation. When func_0021BB00
// says the entity is busy it does nothing. Otherwise, two timer tracks (at
// +0x300 / +0x2FC depending on flag byte +0x234) count up; when a track
// expires it decays the +0x220 blend value by a step (1.0 or 2.0). If the
// blend underflows it forces state 2 and resets; otherwise once the blend is
// small enough it clears the +0x235 bit-0 latch and re-triggers the clip via
// func_0015C9D0.
extern void func_0015C9D0(unsigned char *e);
extern int func_001B0070(void);
extern int func_0021BB00(unsigned char *e);
extern void func_001F0060(int handle, int owner);

extern int D_008106C8;
extern unsigned char D_00810C7E;

void func_0015D100(unsigned char *e) {
    int flag;
    float blend;

    flag = func_001B0070() & 4;
    if (func_0021BB00(e) != 0) {
        return;
    }
    if (*(unsigned char *)(e + 0x234) == 0) {
        if (flag != 4) {
            return;
        }
        if (!(D_008106C8 & 0x60)) {
            return;
        }
        if (D_00810C7E != 0) {
            return;
        }
        *(short *)(e + 0x300) = *(short *)(e + 0x300) + 1;
        if (*(short *)(e + 0x300) < 0x168) {
            return;
        }
        *(short *)(e + 0x300) = 0;
        blend = *(float *)(e + 0x220);
        if (blend <= 1.0f) {
            *(signed char *)(e + 0) = 2;
            *(int *)(e + 0x224) = 0x3F800000;
            *(short *)(e + 0x300) = -0x8000;
            return;
        }
        blend = blend - 1.0f;
        *(float *)(e + 0x220) = blend;
        if (blend <= 35.0f) {
            if (!(*(unsigned char *)(e + 0x235) & 1)) {
                *(unsigned char *)(e + 0x235) = *(unsigned char *)(e + 0x235) & 0xFE;
                *(unsigned char *)(e + 0x235) = *(unsigned char *)(e + 0x235) | 1;
                func_0015C9D0(e);
            }
        }
    } else {
        *(short *)(e + 0x2FC) = *(short *)(e + 0x2FC) + 1;
        if (*(short *)(e + 0x2FC) < 0xF0) {
            return;
        }
        *(short *)(e + 0x2FC) = 0;
        blend = *(float *)(e + 0x220);
        if (blend <= 2.0f) {
            *(signed char *)(e + 0) = 2;
            *(int *)(e + 0x224) = 0x40000000;
            *(signed char *)(e + 0xF) = 0x63;
            return;
        }
        *(float *)(e + 0x220) = blend - 2.0f;
        func_001F0060(0x80000063, 0);
        if (*(float *)(e + 0x220) <= 35.0f) {
            if (!(*(unsigned char *)(e + 0x235) & 1)) {
                *(unsigned char *)(e + 0x235) = *(unsigned char *)(e + 0x235) & 0xFE;
                *(unsigned char *)(e + 0x235) = *(unsigned char *)(e + 0x235) | 1;
                func_0015C9D0(e);
            }
        }
    }
}
