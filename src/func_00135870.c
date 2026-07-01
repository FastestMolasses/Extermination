// NEARMISS func_00135870  (vram 0x00135870, 0x490 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 97.11% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Saved-reg allocation-ORDER permutation: target copies arg1(p1)->s1 THEN arg0(e)->s2 (dispatch byte read via a1); mwcc233 copies e->s2 THEN p1->s1 (dispatch byte read via a0). Same register ASSIGNMENT, opposite emission order, cascading as a uniform a0<->a1 swap through the entire compare chain (3...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// NEARMISS: body/logic fully recovered (entity anim-state machine, states 0-4:
// turn-init, turn-to-target-1, turn-to-target-2 w/ timeout, arrival dispatch +
// wall-avoidance turn kick, cooldown reset). Residual is a genuine saved-reg
// ALLOCATION-ORDER permutation: target copies arg1(p1)->s1 THEN arg0(e)->s2
// (reading the dispatch byte via a1), mwcc233 copies e->s2 THEN p1->s1 (reading
// the dispatch byte via a0) -- same register assignment, opposite emission
// order, cascading as a uniform a0<->a1 swap through the whole compare chain
// (+ matching fa0/fa0f tail on the mtc1). No source-level lever found (param
// decl order, local aliasing) changes the copy-emission order; mwcc233
// measured 97.11%, mwcc(991202) 85.45% (worse: also shifts branch targets).
extern void anim_clip_init(void *p, int idx, float a, float b);
void func_00102760(float *dst, float *src);
void func_00102948(float *dst, float *src);
int func_00122BB8(unsigned char x);
int func_001A7B80(void *p);
float func_001B1240(void *p, float x, float z);
float func_001B12B0(float a, float b, float c);
float func_001B15D0(float *a, float *b);
void func_001FBD50(int a, int b, float c);
int func_0021BE40(unsigned char *p, void *e);

extern unsigned char D_008102B0;
extern float D_00810320;
extern int D_00810324;
extern float D_00810328;
extern float D_00810360;
extern float D_00810368;
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
        anim_clip_init((void *)0xD, 0x5, 0.0f, 0.0f);
        break;
    case 1:
        doTurn = 1;
        if (*(int *)p1 & 0x1000) {
            *(int *)(p1 + 4) = 0;
            func_00102948(&D_700038A0, &D_00810360);
            func_00102948(&D_700038B0, (float *)(e + 0xB0));
            *(int *)0x700038B4 = 0;
            *(int *)0x700038A4 = 0;
            if (func_001B15D0(&D_700038A0, &D_700038B0) <= 50.0f) {
                *(unsigned char *)(e + 6) = 3;
                anim_clip_init(e, 0xF, 0.0f, 0.0f);
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
        func_00102948(&D_700038B0, (float *)(e + 0xB0));
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
            func_001FBD50(0x84E, 0, 300.0f);
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
            *(short *)(p1 + 0x50) = ((func_00122BB8(st) >> 0xB) & 0x1F) + 0x3C;
        }
        break;
    }
    if (doTurn != 0) {
        float ang = func_001B1240((float *)(e + 0xB0), D_00810360, D_00810368);
        *(float *)(p1 + 0x24) = ang;
        *(float *)(e + 0xC4) = func_001B12B0(ang, *(float *)(e + 0xC4), 0.0871557191f);
    }
}
