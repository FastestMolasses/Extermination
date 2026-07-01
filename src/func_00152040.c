// NEARMISS func_00152040  (vram 0x00152040, 0x2E8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.44% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation SAVE-ORDER permutation: with two pointer params both live across calls/switch, mwcc233 always emits paddub s0,arg1 (2nd param) BEFORE paddub s1,arg0 (1st param) in this exact function's control shape, opposite to my emitted s1=arg0/s0=arg1 order (identical shape/order to the b...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Per-state actor/animation driver dispatched on state byte at arg0+6
// (4-way switch 0..3). State 0: bumps state, zeroes arg1+4, sets arg1+0x53 = 1,
// picks arg1+0x2C (0.0f or pi) from arg0+0xB8 < 150.0f, kicks clip 2 via
// anim_clip_init, and if the linked object at arg0+0x20 exists sets its +5
// field to 0xA. State 1: once arg0+0x2C top bit clear, arg1+4==0 and
// arg0+0x3C <= 52.0f, sets arg1+4=1 and fires func_001FBD50(0x88E,0,500.0f);
// blends arg0+0xC4 toward arg1+0x2C via func_001B12B0(goal,cur,rate); once it
// reaches the target and arg1+0 has bit 0x1000 set, advances state and starts
// clip 3. State 2: runs func_0021BE40/func_001A7B80 gating, sets flag bits on
// D_008102B0/D_008102BF, computes D_008104D4 from D_0081070A, builds a
// look-at via func_001B1240 + scratch matrix at 0x70003A20/D_700036A0 and
// func_001029C0/func_00102BB0/func_001026A0, calls func_001B55E0(arg0,1);
// then on arg1+0 bit 0x1000 advances state and starts clip 4. State 3: on
// arg1+0 bit 0x1000, resets arg0+5 and arg0+6 to 0.
extern void anim_clip_init(char *p, int clip, float speed, float d);
extern int func_001026A0(int, int, int);
extern int func_001029C0(void *);
extern int func_00102BB0(void *, void *, float);
extern int func_001A7B80(char *);
extern float func_001B1240(char *, float, float);
extern float func_001B12B0(float, float, float);
extern int func_001B55E0(char *, int);
extern void func_001FBD50(int a, int b, float c);
extern int func_0021BE40(unsigned char *, char *);

extern unsigned char D_008102B0;
extern char D_008102BF;
extern int D_00810320;
extern int D_00810324;
extern int D_00810328;
extern int D_0081032C;
extern float D_00810360;
extern float D_00810368;
extern int D_008104D4;
extern unsigned char D_0081070A;
extern int D_700036A0;

void func_00152040(char *arg0, char *arg1) {
    unsigned char st;
    char *p;

    st = *(unsigned char *)(arg0 + 6);
    switch (st) {
    case 0:
        *(unsigned char *)(arg0 + 6) = st + 1;
        *(int *)(arg1 + 4) = 0;
        *(char *)(arg1 + 0x53) = 1;
        if (*(float *)(arg0 + 0xB8) < 150.0f) {
            *(float *)(arg1 + 0x2C) = 0.0f;
        } else {
            *(float *)(arg1 + 0x2C) = 3.1415927f;
        }
        anim_clip_init(arg0, 2, 5.0f, 0.0f);
        p = *(char **)(arg0 + 0x20);
        if (p != 0) {
            *(char *)(p + 5) = 0xA;
        }
        break;
    case 1:
        if (!(*(short *)(arg0 + 0x2C) & 0x8000) && *(int *)(arg1 + 4) == 0 &&
            *(float *)(arg0 + 0x3C) <= 52.0f) {
            *(int *)(arg1 + 4) = 1;
            func_001FBD50(0x88E, 0, 500.0f);
        }
        *(float *)(arg0 + 0xC4) =
            func_001B12B0(*(float *)(arg1 + 0x2C), *(float *)(arg0 + 0xC4), 0.06981317f);
        if ((*(int *)(arg1 + 0) & 0x1000) &&
            *(float *)(arg0 + 0xC4) == *(float *)(arg1 + 0x2C)) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
            anim_clip_init(arg0, 3, 0.0f, 0.0f);
        }
        break;
    case 2:
        if (func_0021BE40(&D_008102B0, arg0) == 0 && func_001A7B80(arg0) != 0) {
            D_008102B0 |= 2;
            D_008102BF = 2;
            if (D_0081070A != 0) {
                D_008104D4 = 0x41C80000;
            } else {
                D_008104D4 = 0x41A00000;
            }
            *(float *)0x70003A20 = func_001B1240(arg0 + 0xB0, D_00810360, D_00810368);
            func_001029C0(&D_700036A0);
            func_00102BB0(&D_700036A0, &D_700036A0, *(float *)0x70003A20);
            D_00810320 = 0;
            D_00810324 = 0;
            D_00810328 = 0x3F800000;
            D_0081032C = 0x3F800000;
            func_001026A0((int)&D_00810320, (int)&D_700036A0, (int)&D_00810320);
            func_001B55E0(arg0, 1);
        }
        if (*(int *)(arg1 + 0) & 0x1000) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
            anim_clip_init(arg0, 4, 0.0f, 0.0f);
        }
        break;
    case 3:
        if (*(int *)(arg1 + 0) & 0x1000) {
            *(char *)(arg0 + 5) = 0;
            *(char *)(arg0 + 6) = 0;
        }
        break;
    }
}
