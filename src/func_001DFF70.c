// NEARMISS func_001DFF70  (vram 0x001DFF70, 0x178 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.67% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful.
// Matching wave 2026-10-27 (was 91.97%): fixed the callee prototypes (func_001DFE70 takes
// (int, float), func_001D2830 takes (int, int) per their definitions in src/), the case-0
// state store (it sets state 1, not 2), and the entry/FP register colouring (the 0.42 scale in
// its own temp, the 1.0 goal named once, the negation in its own temp, f3 declared late).
// Remaining differences: in case 1 the original rematerialises the constant 1 for the
// done-flag store (compare value in $v0, flag byte in $a0); mwcc here keeps one constant
// register for the compare and the store (2 instructions).
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
//
// Screen fade state machine on the fade block at D_00275670 + 0x1F0: state 0 initialises
// the colour (255, 220, 177, 64) and level 0, then fades in (level eases toward 1.0 at
// 0.4 of the remaining distance plus 1/60 per frame; on reaching 1.0 it sets the done flag);
// state 2 fades out the same way toward 0 and advances to state 3; both draw the fade via
// func_001DFE70(colour, 0.42 * level). State 3 calls func_001D2830(7, 0).

typedef struct {
    unsigned char state;
    unsigned char done;
    short pad;
    float t;
    int pad2[2];
    int col[4];
} Fade;

extern int func_001D2830(int id, int on);
extern void func_001DFE70(int arg0, float arg1);
extern char *D_00275670;

void func_001DFF70(void) {
    Fade *p;
    float k;
    float v;
    float one;
    float f3;
    float neg;

    p = (Fade *)(D_00275670 + 0x1F0);
    switch (p->state) {
    case 0:
        p->t = 0.0f;
        p->col[0] = 0xFF;
        p->col[1] = 0xDC;
        p->col[2] = 0xB1;
        p->col[3] = 0x40;
        p->state = 1;
        /* fallthrough */
    case 1:
        if (p->done != 1) {
            f3 = p->t;
            v = f3 + (0.4f * ((one = 1.0f) - f3) + 0.016666668f);
            p->t = v;
            if (!(v < 1.0f)) {
                p->t = one;
                p->done = 1;
            }
        }
        k = 0.42f * p->t;
        func_001DFE70((int)p->col, k);
        return;
    case 2:
        f3 = p->t;
        neg = -f3;
        v = f3 + (0.4f * neg - 0.016666668f);
        p->t = v;
        if (v < 0.0f) {
            p->t = 0.0f;
            p->state = 3;
        }
        k = 0.42f * p->t;
        func_001DFE70((int)p->col, k);
        return;
    case 3:
        func_001D2830(7, 0);
        return;
    }
}
