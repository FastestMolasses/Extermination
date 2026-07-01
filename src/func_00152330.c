// NEARMISS func_00152330  (vram 0x00152330, 0x324 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 97.96% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Body/structure fully correct (state-machine dispatch, all anim_clip_init calls matched via the FP-ARG-ORDER idiom for the 0x10/0xD/0x11 clip calls, D_008102B0 unsigned-lbu fix). Sole residual: branch-delay-slot filler choice for two independent stores (D_008102B0 |= 2; D_008102BF = 2;) guarding t...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern void anim_clip_init(int self, int clip, float a, float b);
extern void func_001026A0(void *a0, void *a1, void *a2);
extern void func_001029C0(void *a0);
extern void func_00102BB0(void *a0, void *a1, float v);
extern int func_001A7B80(char *a);
extern float func_001B1240(char *a, float b, float c);
extern char func_001B1380(float *a, char *b, int c, float d);
extern void func_001B55E0(char *a, int b);
extern void func_001FBD50(char *p, int a, int b, float f);
extern int func_0021BE40(char *a, char *b, int c);

extern unsigned char D_008102B0;
extern char D_008102BF;
extern float D_00810320;
extern int D_00810324;
extern float D_00810328;
extern float D_0081032C;
extern float D_00810360;
extern float D_00810368;
extern float D_008104D4;
extern unsigned char D_0081070A;
extern char D_700036A0[];

void func_00152330(char *arg0, char *arg1) {
    unsigned char st;
    char *p;
    float z, z2, z3;

    st = *(unsigned char *)(arg0 + 6);
    switch (st) {
    case 0:
        *(unsigned char *)(arg0 + 6) = st + 1;
        *(char *)(arg1 + 0x53) = 2;
        *(float *)(arg1 + 0x28) = 2.0f;
        *(char *)(arg1 + 0x56) = func_001B1380(&D_00810360, arg0 + 0xB0, st, *(float *)(arg0 + 0xC4));
        if (*(char *)(arg1 + 0x56) != 0) {
            anim_clip_init((int)arg0, 0xF, 5.0f, 0.0f);
        } else {
            anim_clip_init((int)arg0, 0xC, 5.0f, 0.0f);
        }
        p = *(char **)(arg0 + 0x20);
        if (p != 0) {
            *(char *)(p + 5) = 0;
        }
        break;
    case 1:
        if (*(int *)arg1 & 0x1000) {
            *(unsigned char *)(arg0 + 6) = st + 1;
            *(float *)(arg1 + 0x28) = 1.0f;
            if (*(char *)(arg1 + 0x56) != 0) {
                anim_clip_init((int)arg0, 0x10, (z = 0.0f), z);
            } else {
                anim_clip_init((int)arg0, 0xD, (z2 = 0.0f), z2);
            }
        }
        break;
    case 2:
        if (*(float *)(arg0 + 0x3C) <= 9.0f && func_0021BE40((char *)&D_008102B0, arg0, st) == 0 && func_001A7B80(arg0) != 0) {
            D_008102B0 |= 2;
            D_008102BF = 2;
            if (D_0081070A != 0) {
                D_008104D4 = 30.0f;
            } else {
                D_008104D4 = 25.0f;
            }
            *(float *)0x70003A20 = func_001B1240(arg0 + 0xB0, D_00810360, D_00810368);
            func_001029C0(&D_700036A0);
            func_00102BB0(&D_700036A0, &D_700036A0, *(float *)0x70003A20);
            D_00810320 = 0.0f;
            D_00810324 = 0;
            D_00810328 = 1.0f;
            D_0081032C = 1.0f;
            func_001026A0(&D_00810320, &D_700036A0, &D_00810320);
            func_001B55E0(arg0, 1);
        }
        if (*(int *)arg1 & 0x1000) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
            if (*(char *)(arg1 + 0x56) != 0) {
                anim_clip_init((int)arg0, 0x11, (z3 = 0.0f), z3);
                p = *(char **)(arg0 + 0x20);
                if (p != 0) {
                    *(char *)(p + 5) = 0xB;
                }
            } else {
                anim_clip_init((int)arg0, 0xE, 0.0f, 0.0f);
                p = *(char **)(arg0 + 0x20);
                if (p != 0) {
                    *(char *)(p + 5) = 0xC;
                }
            }
            func_001FBD50(arg0, 0x88B, 0, 500.0f);
        }
        break;
    case 3:
        if (*(int *)arg1 & 0x1000) {
            *(char *)(arg0 + 5) = 0;
            *(unsigned char *)(arg0 + 6) = 0;
            *(char *)(arg1 + 0x57) = *(char *)(arg1 + 0x57) + 1;
        }
        break;
    }
}
