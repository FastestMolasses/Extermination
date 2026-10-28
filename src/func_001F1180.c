// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 81.62% NEARMISS).
// Fixes read from the original instructions:
//  - The state dispatch is one switch (case 0 / case 1, cases end in break);
//    the colour-table selection is a switch on the glow type with a nested
//    switch on D_00810700 (default first, then 1, then 2).
//  - The visibility test was inverted in the old C: for types 1, 2 and 4 the
//    glow is drawn only when the facing dot product is <= 0, every other type
//    is always drawn; a hidden glow clears the state word (+0x0C). Written as
//    a switch with cases 1/2/4 and a default.
//  - func_0011E2A8 takes only the float (no int first argument).
//  - The two colour words are built with |= statements (first word first),
//    the angle and phase updates use compound assignments, and the stack
//    vectors and the long-lived locals are declared at the top.
//
// Glow sprite driver on the actor's glow block (arg0+0x2D0: angle, phase,
// type, sub-index, state). State 0: count the phase down by 1 per call; below 0
// pick a random sub-index (mod 4) and go to state 1. State 1: select a 0x2C-byte
// colour entry (type 1 picks one of three tables by D_00810700 and the
// sub-index; other types index D_00259DD0 by type), test visibility, then
// draw through func_001F0A60 with alpha/size faded by func_0011E2A8(PI*phase),
// spin the angle by 6 degrees (wrapping past 180), and advance the phase by
// 0.05; past 1.0 reload the phase with 40 + rand()%120 and return to state 0.
typedef struct {
    float angle;
    float phase;
    short type;
    short sub;
    int state;
} Glow;

extern void func_001026A0(void *dst, void *matrix, void *vec);
extern float func_00102738(void *a, void *b);
extern void func_00102760(void *dst, void *src);
extern void func_001028D0(void *dst, void *a, void *b);
extern float func_0011E2A8(float f);
extern int func_00122BB8(void);
extern int float_to_int(float f);
extern void func_001F0A60(int chain_no, int mode, float *pos, unsigned int inner, unsigned int outer,
                         float angle, float size, float depth);
extern char D_00259DD0[];
extern char D_00259EE0[];
extern char D_00259F90[];
extern char D_0025A040[];
extern char D_008105D0[];
extern unsigned char D_00810700;
void func_001F1180(char *arg0) {
    int inner;
    int outer;
    char *ent;
    Glow *g;
    float fade;
    float size;
    int vis;
    short type;
    float sp70[4];
    float sp80[4];

    g = (Glow *)(arg0 + 0x2D0);
    switch (g->state) {
    case 0:
        g->phase = g->phase - 1.0f;
        if (g->phase < 0.0f) {
            g->phase = 0.0f;
            g->sub = func_00122BB8() % 4;
            g->state = 1;
        }
        break;
    case 1:
        type = g->type;
        switch (type) {
        case 1:
            switch (D_00810700) {
            default:
                ent = D_00259EE0 + g->sub * 0x2C;
                break;
            case 1:
                ent = D_00259F90 + g->sub * 0x2C;
                break;
            case 2:
                ent = D_0025A040 + g->sub * 0x2C;
                break;
            }
            break;
        default:
            ent = D_00259DD0 + type * 0x2C;
            break;
        }
        switch (type) {
        case 1:
        case 2:
        case 4:
            func_001028D0(sp80, arg0 + 0x100, D_008105D0);
            func_00102760(sp80, sp80);
            func_00102760(sp70, arg0 + 0xF0);
            vis = func_00102738(sp70, sp80) <= 0.0f;
            break;
        default:
            vis = 1;
            break;
        }
        if (!vis) {
            g->state = 0;
        } else {
            fade = func_0011E2A8(3.1415927f * g->phase);
            size = *(float *)(ent + 0x24) * fade;
            inner = *(int *)(ent + 0);
            inner |= *(int *)(ent + 4) << 8;
            inner |= *(int *)(ent + 8) << 16;
            outer = *(int *)(ent + 0xC);
            outer |= *(int *)(ent + 0x10) << 8;
            outer |= *(int *)(ent + 0x14) << 16;
            inner |= float_to_int(128.0f * fade) << 24;
            outer |= float_to_int(16.0f * fade) << 24;
            sp70[0] = *(float *)(ent + 0x18);
            sp70[1] = *(float *)(ent + 0x1C);
            sp70[2] = *(float *)(ent + 0x20);
            sp70[3] = 1.0f;
            func_001026A0(sp70, arg0 + 0xD0, sp70);
            func_001F0A60(0, 1, sp70, inner, outer, (3.1415927f * g->angle) / 180.0f, size, *(float *)(ent + 0x28));
            g->angle += 6.0f;
            if (!(g->angle <= 180.0f)) {
                g->angle -= 360.0f;
            }
            g->phase += 0.05f;
            if (!(g->phase <= 1.0f)) {
                g->phase = 40.0f + (float)(func_00122BB8() % 120);
                g->state = 0;
            }
        }
        break;
    }
}
