// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x70003B8D

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 93.18% NEARMISS).
// Fixes read from the original instructions:
//  - Callee prototypes taken from their definitions in src/: func_0011DF78
//    takes one float, func_001AFC10 takes the entity, func_001EFD20 takes two
//    arguments, func_001B41F0 takes six (extra 0x2000, 0x50). The "a0 vs a1
//    constant" residual was the switch's compare constants left in the
//    argument registers, not a compiler artifact.
//  - Every switch case ends in `break` (single return after the switch).
//  - The 0x28 countdown tests the decremented short temp, not a reload.
//  - Float updates spelled as compound assignments (+=, -=).
//  - The collision result is assigned inside the if condition.
//  - 0x70003B8D is a relocated volatile scratchpad extern (idiom-32).

// Entity state machine (states 0-3: turn-init with TRS setup, homing-turn-
// and-collision-response with knockback blend + camera-shake trigger, hit
// react, idle-passthrough). Includes the nested switch (turn-speed blend vs
// 1/3-decay), the collision-response dispatch (func_0019A570 hit-kind branch
// incl. the masked-flag/kind==1 special case and the func_0019B6C0 fallback +
// camera-shake-45 check), and the final TRS-matrix build.
extern void build_trs_matrix(char *dst, char *pos, float *rot, float *scale);
extern void func_00102760(void *dst, void *src);
extern void func_001028D0(void *a, void *b, void *c);
extern void func_00102948(void *dst, void *src);
extern void func_001031E0(void *a, void *b);
extern void func_00103200(void *dst, void *a, void *b, float c);
extern void func_00103230(char *a, char *b, float c);
extern float func_0011DF78(float a);
extern void func_00183C40(int idx, void *dst);
extern int func_0019A570(char *a, char *b, int c, int d);
extern int func_0019B6C0(char *a, char *b);
extern void func_001AFC10(char *p);
extern void func_001B41F0(char *e, void *a, char *b, int c, int d, int f);
extern void func_001E8B90(char *p, float f);
extern void func_001EFD20(int a, char *b);
extern char *func_001EFE00(int a, char *e);
extern void func_001FBD50(char *e, int a, int b, float c);

extern float D_700031B0;
extern volatile unsigned char D_70003B8D;
extern float D_700038A0;
extern float D_700038B0;

void func_0018AF50(char *e) {
    unsigned char st = *(unsigned char *)(e + 4);
    int r;
    char *q;
    unsigned char kind;
    short t;

    switch (st) {
    case 0:
        *(unsigned char *)(e + 4) = st + 1;
        *(short *)(e + 0x28) = 0x32;
        func_00103230(e + 0xA0, e + 0xC0, 6.0f);
        func_001028D0(e + 0xA0, e + 0xB0, e + 0xA0);
        func_00103230(e + 0xC0, e + 0xC0, 4.0f);
        *(char **)(e + 0x20) = func_001EFE00(0x80000037, e);
        *(float *)(e + 0x38) = 0.0f;
        *(short *)(e + 0x2A) = 0x12C;
        break;
    case 1:
        if (D_70003B8D != 0) {
            *(char *)(*(char **)(e + 0x20) + 4) = 2;
            *(unsigned char *)(e + 4) = 3;
            break;
        }
        if (*(short *)(e + 0x2A) == 0) {
            *(unsigned char *)(e + 4) = 2;
        } else {
            *(short *)(e + 0x2A) = *(short *)(e + 0x2A) - 1;
        }
        switch (*(unsigned char *)(e + 5)) {
        case 0:
            t = *(short *)(e + 0x28) - 1;
            *(short *)(e + 0x28) = t;
            if (t == 0) {
                *(unsigned char *)(e + 5) = *(unsigned char *)(e + 5) + 1;
                if (*(float *)(e + 0xC4) < 0.0f) {
                    *(float *)(e + 0x38) = func_0011DF78(*(float *)(e + 0xC4));
                }
            }
            if (*(int *)(e + 0x24) != 0) {
                func_00183C40(*(int *)(e + 0x24), &D_700038B0);
                func_001028D0(&D_700038A0, &D_700038B0, e + 0xB0);
                func_00102760(&D_700038A0, &D_700038A0);
                func_00103200(&D_700038B0, &D_700038A0, e + 0xC0, 0.2f);
                func_00102760((float *)(e + 0xC0), &D_700038B0);
                func_00103230(e + 0xC0, e + 0xC0, 4.0f);
            }
            *(float *)(e + 0xB0) += *(float *)(e + 0xC0);
            *(float *)(e + 0xB4) += *(float *)(e + 0xC4);
            *(float *)(e + 0xB8) += *(float *)(e + 0xC8);
            break;
        case 1:
            *(float *)(e + 0xB0) += *(float *)(e + 0xC0) / 3.0f;
            *(float *)(e + 0xB8) += *(float *)(e + 0xC8) / 3.0f;
            *(float *)(e + 0x38) += 0.03f;
            if (!(*(float *)(e + 0x38) <= 4.0f)) {
                *(float *)(e + 0x38) = 4.0f;
            }
            *(float *)(e + 0xB4) -= *(float *)(e + 0x38);
            break;
        }
        if ((r = func_0019A570(e + 0xA0, e + 0xB0, 7, 0x20)) != 0) {
            func_001031E0(&D_700038A0, &D_700031B0);
            *(int *)0x700038AC = 0x3F800000;
            func_00102948(e + 0xB0, &D_700038A0);
            if ((unsigned int)(r - 1) < 2U) {
                q = *(char **)0x700031D4;
                if (q != 0) {
                    kind = *(unsigned char *)q;
                    if (kind != 0) {
                        if ((*(unsigned char *)(q + 2) & ~0xE0) == 2) {
                            if (kind == 1) {
                                func_001B41F0(q, &D_700038A0, e + 0xC0, *(int *)(*(char **)0x700031D0 + 0x1C), 0x2000, 0x50);
                            }
                        } else {
                            *(short *)(q + 0x36) = 0x50;
                            func_00102948(q + 0x70, e + 0xC0);
                        }
                    }
                }
            }
            *(unsigned char *)(e + 4) = 2;
        } else if (func_0019B6C0(e + 0xA0, e + 0xB0) != 0) {
            func_001031E0(e + 0xB0, &D_700031B0);
            if (*(unsigned char *)(*(char **)0x700031D0 + 0x1A) == 0x5B) {
                func_001E8B90(e + 0xB0, 4.0f);
            }
            *(unsigned char *)(e + 4) = 2;
        }
        *(float *)(e + 0xA0) = *(float *)(e + 0xB0);
        *(float *)(e + 0xA4) = *(float *)(e + 0xB4);
        *(float *)(e + 0xA8) = *(float *)(e + 0xB8);
        *(float *)(e + 0xAC) = *(float *)(e + 0xBC);
        *(int *)0x700038A0 = 0;
        *(int *)0x700038A4 = 0;
        *(int *)0x700038A8 = 0;
        *(int *)0x700038AC = 0x3F800000;
        build_trs_matrix(e + 0xD0, e + 0xB0, &D_700038A0, &D_700038A0);
        break;
    case 2:
        *(unsigned char *)(e + 4) = st + 1;
        *(char *)(*(char **)(e + 0x20) + 4) = 2;
        func_001EFD20(0x80000069, e + 0xB0);
        func_001FBD50(e, 0x5DE, 0, 300.0f);
        break;
    case 3:
        func_001AFC10(e);
        break;
    }
}
