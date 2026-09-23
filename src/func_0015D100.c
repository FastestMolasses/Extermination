// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Player passive health drain (FINDINGS "Passive ticks (state-1 tail)"; e is
// the player struct: +0x220 health, +0x234 infected latch, +0x235 bit 0 the
// low-health latch). Returns at once when the func_0021BB00 anim gate says
// so. Not infected (+0x234 == 0): only with area flag bit 2
// (func_001B0070() & 4), D_008106C8 & 0x60 and D_00810C7E == 0, the +0x300
// counter ticks; every 360 frames health -= 1.0 (at <= 1.0 instead: event
// byte +0 = 2, pending damage +0x224 = 1.0, +0x300 = -0x8000). Infected: the
// +0x2FC counter ticks; every 240 frames health -= 2.0 with effect
// func_001F0060(0x80000063, 0) (at <= 2.0 instead: event 2, +0x224 = 2.0,
// +0xF = 0x63). In both paths, when health is then <= 35.0 and +0x235 bit 0 is
// still clear, it SETS that latch (&0xFE then |1) and calls func_0015C9D0
// (clip re-trigger).
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
