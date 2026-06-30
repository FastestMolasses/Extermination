// NEARMISS func_0012F100  (vram 0x0012F100, 0x5BC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 91.68% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Scheduling/regalloc permutation. Body fully recovered and structurally byte-identical (367==367 instructions); residual diffs are mwcc register-coloring in the D_00244F30 table-index calc, float-arg load scheduling around func_001B1240/13F0, and a branch-sense (beqz/bnez) flip on the func_001B13F...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p

//
// NEARMISS (mwcc 2.3.3, 91.7% objdiff; 991202 86.3%). Body fully recovered:
// 3-state actor tick keyed on p[6] (a0). State 0 seeds anim fields, RNG-picks
// an anim row from table D_00244F30 via func_00122BB8 and starts a clip; state
// 1 gates on the e[0x58] flag then advances; state 2 runs the steering/contact
// update (func_001333F0/func_00132FB0, scratch-pad matrix build at
// 0x700038A0/B0, func_001B1240/13F0/1560/3250). Shared tail decrements the
// e+0x50 timer and the e+0x54 SFX timer (firing func_001EFD90 0x8000001D).
// Instruction stream is structurally identical (367==367 insns); residual is
// mwcc register-coloring + float-arg load scheduling + a branch-sense flip on
// the func_001B13F0 guards (scheduling/regalloc permutation, permuter class).
extern int func_00122BB8(void);
extern float func_001B1470(float);
extern void anim_clip_init(int self, int clip, float a, float b);
extern void func_001FBD50(int self, int a, int b, float c);
extern int func_001333F0(unsigned char *p, unsigned char *e);
extern int func_00132FB0(unsigned char *p, unsigned char *e);
extern int func_001B13F0(float r, void *a, void *b);
extern float func_001B1240(unsigned char *origin, float x, float z);
extern void func_00131F20(unsigned char *p, void *a, void *b);
extern void func_001028B8(void *a, void *b, void *c);
extern int func_001B3250(float r, void *v);
extern int func_001B1560(unsigned char *p, void *v, float r);
extern void func_00133640(unsigned char *p, unsigned char *e);
extern void func_00131F90(unsigned char *p, unsigned char *e);
extern void func_001EFD90(int mode, void *b, void *c);
extern void func_00132490(unsigned char *p, unsigned char *e);

extern unsigned short D_00244F30;
extern float D_00810360;
extern float D_00810368;
extern char D_700038A0[];
extern char D_700038B0[];

void func_0012F100(unsigned char *p, unsigned char *e) {
    int t;

    switch (p[6]) {
    case 0:
        e[0x63] = 0;
        *(short *)(e + 0x54) = 0;
        *(int *)(e + 0x3C) = 0;
        *(float *)(e + 0x30) =
            func_001B1470(6.2831855f * (float)((func_00122BB8() >> 13) & 0xFF) / 255.0f);
        t = func_00122BB8();
        *(unsigned short *)(e + 0x50) =
            *(unsigned short *)((char *)&D_00244F30 + (e[0x61] << 5) + (((t >> 7) & 0xF) << 1));
        if (e[0x61] != 0) {
            if (*(short *)(p + 0x2C) == 2) {
                p[6] = 2;
                *(int *)(e + 0x3C) = 0x3DCCCCCD;
            } else {
                t = p[6] + 1;
                anim_clip_init((int)p, t, 10.0f, 0.0f);
                p[6] = t;
            }
        } else {
            if (*(short *)(p + 0x2C) == 7) {
                p[6] = 2;
            } else {
                t = p[6] + 1;
                anim_clip_init((int)p, t, 10.0f, 0.0f);
                p[6] = t;
            }
        }
        break;
    case 1:
        if (*(unsigned short *)(e + 0x58) & 0x8000) {
            break;
        }
        p[6] = p[6] + 1;
        *(int *)(e + 0x3C) = 0x3DCCCCCD;
        /* fall through */
    case 2:
        if (e[0x61] != 0) {
            if (*(float *)(p + 0x3C) == 95.0f || *(float *)(p + 0x3C) == 43.0f) {
                func_001FBD50((int)p, 0x7DE, 0, 300.0f);
            }
        }
        if (func_001333F0(p, e) == 0 && (p[0xD] & 3) == 3 && e[0x61] == 0) {
            p[5] = 9;
            p[6] = 0;
            break;
        }
        if (*(unsigned short *)(e + 0x56) != 0 && e[0x61] != 0) {
            p[5] = 0;
            p[6] = 0;
        }
        if (e[0x62] != 0) {
            e[0x62] = e[0x62] - 1;
            if (*(float *)(e + 0x30) == *(float *)(p + 0xC4)) {
                e[0x62] = 0;
            }
        } else if (func_00132FB0(p, e) == 0) {
            if (*(unsigned short *)(e + 0x56) != 0 && e[0x61] == 0) {
                if (func_001B13F0(15.0f, &D_00810360, p + 0xB0) == 0) {
                    *(float *)(e + 0x30) =
                        func_001B1240(p + 0xB0, D_00810360, D_00810368);
                }
                if (func_001B1560(p, &D_00810360, 0.034906585f) != 0) {
                    *(float *)0x700038A0 = 3.0f;
                    *(float *)0x700038A4 = 70.0f;
                    *(float *)0x700038A8 = 1.0f;
                    func_00131F20(p, D_700038A0, D_700038A0);
                    func_001028B8(D_700038A0, D_700038A0, p + 0xB0);
                    if (func_001B3250(15.0f, D_700038A0) == 0) {
                        p[5] = 2;
                        p[6] = 0;
                    }
                }
            } else {
                if (((*(int *)0x70003B68 + *(short *)0x70003B8A) & 0x3F) == 0) {
                    *(float *)0x700038A0 = *(float *)(p + 0xB0);
                    *(float *)0x700038A4 = *(float *)(p + 0xB4);
                    *(float *)0x700038A8 = *(float *)(p + 0xB8);
                    *(float *)0x700038AC = *(float *)(p + 0xBC);
                    *(float *)0x700038B0 = *(float *)(e + 0x0);
                    *(float *)0x700038B4 = *(float *)(e + 0x4);
                    *(float *)0x700038B8 = *(float *)(e + 0x8);
                    *(float *)0x700038BC = *(float *)(e + 0xC);
                    *(int *)0x700038B4 = 0;
                    *(int *)0x700038A4 = 0;
                    if (e[0x68] != 0) {
                        if (func_001B13F0(15.0f, D_700038A0, D_700038B0) != 0) {
                            e[0x68] = 0;
                        }
                        *(float *)(e + 0x30) =
                            func_001B1240(p + 0xB0, *(float *)(e + 0x0), *(float *)(e + 0x8));
                    } else if (func_001B13F0(30.0f, D_700038A0, D_700038B0) == 0) {
                        e[0x68] = 1;
                    }
                }
            }
        }
        /* .L0012F5C0 tail */
        *(short *)(e + 0x50) = *(short *)(e + 0x50) - 1;
        if ((unsigned short)*(short *)(e + 0x50) == 0) {
            p[5] = 0;
            p[6] = 0;
        }
        if (p[5] == 1) {
            func_00133640(p, e);
        }
        func_00131F90(p, e);
        break;
    }

    if (e[0x69] & 1) {
        if (*(unsigned short *)(e + 0x54) != 0) {
            *(short *)(e + 0x54) = *(short *)(e + 0x54) - 1;
        } else {
            *(short *)(e + 0x54) = ((func_00122BB8() >> 9) & 0xF) + 7;
            *(float *)0x700038A0 = *(float *)(p + 0xB0);
            *(float *)0x700038A4 = *(float *)(p + 0xB4);
            *(float *)0x700038A8 = *(float *)(p + 0xB8);
            *(int *)0x700038AC = 0x3F800000;
            *(float *)0x700038A4 = *(float *)(e + 0x40);
            func_001EFD90(0x8000001D, D_700038A0, p + 0xC0);
        }
    }
    func_00132490(p, e);
}
