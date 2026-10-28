// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 94.41% NEARMISS).
// Fixes read from the original instructions:
//  - Every state ends in `break` (one shared exit), not `return`; case 0x63 is an
//    explicit empty case after case 3 (the dispatch compares 0x63 first).
//  - State 1 passes the constant 1 to func_0016BAE0 (the dispatch's compare constant
//    is reused as the argument), not the state byte.
//  - func_0016BAE0 / func_001FBD50 prototypes now agree with their definitions in src/;
//    the 0xB4 integration is a compound assignment.

// Per-state weapon/anim driver dispatched on the state byte at arg0+6
// (switch 0/1/2/3/0x63). State 0: eases the 0xC4 angle toward the 0x218 goal
// via func_001B12B0 (rate pi/8); once settled, either latches to state 0x63
// when the global mode D_00810700==0x11 (also sets D_00810835=1), or advances:
// bumps state, starts clip 0x17F at blend 8.0, calls func_00182870, then per
// func_0016BB80 computes the 0x254/0x258 sweep range. State 1: if the 0x8000
// flag at arg0+0x200 is set, integrates 0xB4 += 0x258/8; otherwise advances
// (func_0016BAE0, clears D_00275B00[2], sets 0x25F=1, 0xB4=0x254). State 2:
// once 0x3C <= 46.0, advances and kicks func_001FBD50(...,300.0). State 3:
// when the 0x1000 flag is set, transitions (5=0x17, 6=0, 0x1F0=0x2A).
extern float func_001B12B0(float goal, float cur, float rate);
extern int func_001749A0(char *e, int clip, int flags, float blend);
extern void func_00182870(char *e, int f);
extern int func_0016BB80(char *e);
extern void func_0016BAE0(unsigned char *a, unsigned char b);
extern int func_001FBD50(void *p, int a, int b, float f);
extern int D_00275B00;
extern unsigned char D_00810700[16];
extern char D_00810835[16];

void func_0016BC40(char *arg0)
{
    unsigned char st;

    st = *(unsigned char *)(arg0 + 6);
    switch (st) {
    case 0:
        *(float *)(arg0 + 0xC4) = func_001B12B0(*(float *)(arg0 + 0x218), *(float *)(arg0 + 0xC4), 0.39269909f);
        if (*(float *)(arg0 + 0xC4) == *(float *)(arg0 + 0x218)) {
            if (D_00810700[0] == 0x11) {
                *(unsigned char *)(arg0 + 6) = 0x63;
                D_00810835[0] = 1;
            } else {
                *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
                func_001749A0(arg0, 0x17F, 0, 8.0f);
                func_00182870(arg0, 0);
                if (func_0016BB80(arg0) != 0) {
                    float t = *(float *)0x700031B4 - 22.5f;
                    *(float *)(arg0 + 0x254) = t;
                    *(float *)(arg0 + 0x258) = t - *(float *)(arg0 + 0xB4);
                } else {
                    *(float *)(arg0 + 0x254) = *(float *)(arg0 + 0xB4);
                    *(int *)(arg0 + 0x258) = 0;
                }
            }
        }
        break;
    case 1:
        if ((*(int *)(arg0 + 0x200) & 0x8000) == 0) {
            *(unsigned char *)(arg0 + 6) = st + 1;
            func_0016BAE0((unsigned char *)arg0, 1);
            (&D_00275B00)[2] = 0;
            *(char *)(arg0 + 0x25F) = 1;
            *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0x254);
        } else {
            *(float *)(arg0 + 0xB4) += *(float *)(arg0 + 0x258) / 8.0f;
        }
        break;
    case 2:
        if (*(float *)(arg0 + 0x3C) <= 46.0f) {
            *(unsigned char *)(arg0 + 6) = st + 1;
            func_001FBD50(arg0, 0x134, 0, 300.0f);
        }
        break;
    case 3:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            *(char *)(arg0 + 5) = 0x17;
            *(unsigned char *)(arg0 + 6) = 0;
            *(char *)(arg0 + 0x1F0) = 0x2A;
        }
        break;
    case 0x63:
        break;
    }
}
