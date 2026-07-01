// NEARMISS func_00139240  (vram 0x00139240, 0x7A8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.15% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FP-companion register coloring on direct lwc1 loads (documented un-leverable class): in the two vec4 scratchpad staging blocks the target pairs the four D_00810360/364/368/36C loads into f0-f3 (odd f1/f3 companion halves) with the pair-copy temp on f4; mwcc233 spreads them to f2/f4/f5/f6 with the...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Enemy AI think (states 0/1 of the state byte arg0+6; ctx arg1).
// dist = func_001B15D0(arg0+0xB0, &D_00810360) = distance to the player pos.
// State 0 (enter): advances to state 1, zeroes the ctx timers (+0x24/+0x22/
// +0x2C/+0x30), seeds speed ramp +0x4C = 0.65; if dist > 50 picks a behavior
// byte from the per-side table D_002451A0[side*0x10 + rand&0xF] (side = bit7
// of arg0+0xD): 0 -> expire (flag), 1 -> target speed +0x40 = 0.4, else 0.8.
// State 1: when not staggered (arg1+0x80 == 0), close (dist <= 50), unblocked
// (func_0021BE40 == 0) and facing within 10 deg (func_001B1560, 0.1745 rad):
// consults D_002451C0[side*0x10 + rand&0xF]; on 5 stages the ctx position vs
// player (scratch vec4s D_700038A0/D_700038B0, y zeroed) and if the planar
// range > 20 goes to attack state 5 (arg0+5=5) else expires; other rolls go
// to state 4. Otherwise: if dist > 100 counts frames at +0x22 and expires at
// 241; else (<= 100) resets that counter and, when dist > 50 with flag bit7
// of arg0+0xA set and +0x32 clear and facing ok, goes to approach state 3.
// Common tail: eases the speed +0x44 toward the +0x40 target (accel +0x48 =
// -0.005 when staggered above 0.4, +0.001 below target, 0 at clamp); when the
// re-aim timer +0x24 expires and not stunned (arg1+0x80 & 0xC), recomputes the
// heading +0x58 (func_001B1240) and, if planar range <= 15 and not facing
// (func_001B1560, 1.4835 rad), re-arms the timer to 60+rand&0x7F; the wander
// timer +0x20 (forced to 120 while flags&3) picks a new wander angle +0x5C =
// 3.1067*(rand/255)-1.5533 when it expires, mirrored away from walls
// (func_001B2F70 probe / height guards vs D_00810364) via func_0011DF78 or
// *-1; the life counter +0x2C expires the state machine at 301 frames
// (reset to state 1 with +0x2E = 120+rand&0x3F). Finally eases the model yaw
// arg0+0xC4 toward +0x58 (rate pi/120) and the wander heading +0x50 toward
// +0x5C (rate pi*(1+1.8*rand/255)/180), then func_0013BBB0/func_0013BA20
// (move + animate).
//
// NEARMISS 99.15% on mwcc 2.3.3 (91.93% on pinned 991202). Logic/structure
// fully recovered; residuals are pure register-coloring artifacts: (1) the two
// vec4 scratchpad staging blocks - the target pairs the four D_008103xx loads
// into f0-f3 (odd f1/f3 companions, direct-lwc1 FP-companion coloring, the
// documented un-leverable class); mwcc233 spreads them to f2/f4/f5/f6 with the
// pair-copy temp on f0 instead of f4; (2) the two timeout compares emit
// "slti v0" where the target holds "slti at" (233 tie-break; 991202 gives $at
// but sltiu). Permuter territory; not a clean-store delay-slot nop.

extern float func_001B15D0(void *pos, void *target);
extern void func_0013C8C0(char *a, char *b);
extern int func_00122BB8(void);
extern int func_0021BE40(void *a, char *b);
extern int func_001B1560(char *a, void *b, float ang);
extern float func_001B1240(void *pos, float x, float z);
extern int func_001B2F70(void *pos, void *v);
extern float func_0011DF78(float a);
extern float func_001B12B0(float goal, float cur, float rate);
extern void func_0013BBB0(char *a, char *b);
extern void func_0013BA20(char *a, char *b);

extern char D_002451A0[];
extern char D_002451C0[];
extern char D_008102B0;
extern float D_00810360;
extern float D_00810364;
extern float D_00810368;
extern float D_0081036C;
extern char D_700038A0[];
extern char D_700038B0[];

void func_00139240(char *arg0, char *arg1) {
    int flag;
    int ix;
    int tbl;
    int c;
    unsigned short t;
    float dist;
    float r;
    float w;
    float g3, g2, g1, g0;
    unsigned char st;

    flag = 0;
    dist = func_001B15D0(arg0 + 0xB0, &D_00810360);
    func_0013C8C0(arg0, arg1);

    st = *(unsigned char *)(arg0 + 6);
    switch (st) {
    case 0:
        *(unsigned char *)(arg0 + 6) = st + 1;
        *(unsigned short *)(arg1 + 0x24) = 0;
        *(unsigned short *)(arg1 + 0x22) = 0;
        *(float *)(arg1 + 0x4C) = 0.65f;
        *(unsigned short *)(arg1 + 0x2C) = 0;
        *(short *)(arg1 + 0x30) = 0;
        if (dist > 50.0f) {
            tbl = (int) D_002451A0 + (((*(unsigned char *)(arg0 + 0xD) & 0x80) >> 7) * 0x10);
            ix = (func_00122BB8() >> 11) & 0xF;
            c = *(char *)(ix + tbl);
            if (c == 0) {
                flag = 1;
            } else {
                if ((char) c == 1) {
                    *(float *)(arg1 + 0x40) = 0.4f;
                } else {
                    *(float *)(arg1 + 0x40) = 0.8f;
                }
                flag = 0;
            }
        }
        break;
    case 1:
        if (*(char *)(arg1 + 0x80) == 0
            && dist <= 50.0f
            && func_0021BE40(&D_008102B0, arg0) == 0
            && func_001B1560(arg0, &D_00810360, 0.174532935f) != 0) {
            tbl = (int) D_002451C0 + (((*(unsigned char *)(arg0 + 0xD) & 0x80) >> 7) * 0x10);
            ix = (func_00122BB8() >> 15) & 0xF;
            if (*(char *)(ix + tbl) == 5) {
                g0 = D_00810360;
                g1 = D_00810364;
                g2 = D_00810368;
                g3 = D_0081036C;
                *(float *)0x700038A0 = *(float *)(arg0 + 0xB0);
                *(float *)0x700038A4 = *(float *)(arg0 + 0xB4);
                *(float *)0x700038A8 = *(float *)(arg0 + 0xB8);
                *(float *)0x700038AC = *(float *)(arg0 + 0xBC);
                *(float *)0x700038B0 = g0;
                *(float *)0x700038B4 = g1;
                *(float *)0x700038B8 = g2;
                *(float *)0x700038BC = g3;
                *(int *)0x700038B4 = 0;
                *(int *)0x700038A4 = 0;
                if (func_001B15D0(D_700038A0, D_700038B0) > 20.0f) {
                    *(char *)(arg0 + 5) = 5;
                    *(unsigned char *)(arg0 + 6) = 0;
                } else {
                    flag = 1;
                }
            } else {
                *(char *)(arg0 + 5) = 4;
                *(unsigned char *)(arg0 + 6) = 0;
            }
        } else {
            if (dist > 100.0f) {
                c = *(unsigned short *)(arg1 + 0x22) + 1;
                *(unsigned short *)(arg1 + 0x22) = c;
                if ((c & 0xFFFF) >= 0xF1) {
                    flag = 1;
                }
            } else {
                *(unsigned short *)(arg1 + 0x22) = 0;
                if (dist > 50.0f
                    && (*(unsigned char *)(arg0 + 0xA) & 0x80)
                    && *(unsigned short *)(arg1 + 0x32) == 0
                    && func_001B1560(arg0, &D_00810360, 0.174532935f) != 0) {
                    *(char *)(arg0 + 5) = 3;
                    *(unsigned char *)(arg0 + 6) = 0;
                    *(int *)(arg0 + 0xC8) = 0;
                    *(int *)(arg1 + 0x48) = 0;
                }
            }
        }
        break;
    }

    if (*(char *)(arg1 + 0x80) != 0) {
        if (*(float *)(arg1 + 0x44) > 0.4f) {
            *(float *)(arg1 + 0x48) = -0.005f;
        } else {
            *(float *)(arg1 + 0x44) = 0.4f;
            *(float *)(arg1 + 0x48) = 0.0f;
        }
    } else {
        if (*(float *)(arg1 + 0x44) < *(float *)(arg1 + 0x40)) {
            *(float *)(arg1 + 0x48) = 0.001f;
        } else {
            *(float *)(arg1 + 0x44) = *(float *)(arg1 + 0x40);
            *(float *)(arg1 + 0x48) = 0.0f;
        }
    }

    t = *(unsigned short *)(arg1 + 0x24);
    if (t != 0) {
        *(unsigned short *)(arg1 + 0x24) = t - 1;
    } else if ((*(char *)(arg1 + 0x80) & 0xC) == 0) {
        *(float *)(arg1 + 0x58) = func_001B1240(arg0 + 0xB0, D_00810360, D_00810368);
        g0 = D_00810360;
        g1 = D_00810364;
        g2 = D_00810368;
        g3 = D_0081036C;
        *(float *)0x700038A0 = *(float *)(arg0 + 0xB0);
        *(float *)0x700038A4 = *(float *)(arg0 + 0xB4);
        *(float *)0x700038A8 = *(float *)(arg0 + 0xB8);
        *(float *)0x700038AC = *(float *)(arg0 + 0xBC);
        *(float *)0x700038B0 = g0;
        *(float *)0x700038B4 = g1;
        *(float *)0x700038B8 = g2;
        *(float *)0x700038BC = g3;
        *(int *)0x700038B4 = 0;
        *(int *)0x700038A4 = 0;
        if (func_001B15D0(D_700038A0, D_700038B0) <= 15.0f
            && func_001B1560(arg0, &D_00810360, 1.48352981f) == 0) {
            *(unsigned short *)(arg1 + 0x24) = ((func_00122BB8() >> 6) & 0x7F) + 0x3C;
        }
    }

    if (*(char *)(arg1 + 0x80) & 3) {
        *(unsigned short *)(arg1 + 0x20) = 0x78;
    } else {
        t = *(unsigned short *)(arg1 + 0x20);
        if (t != 0) {
            *(unsigned short *)(arg1 + 0x20) = t - 1;
        } else {
            *(unsigned short *)(arg1 + 0x20) = ((func_00122BB8() >> 3) & 0x3F) + 0x3C;
            *(float *)(arg1 + 0x5C) =
                3.10668612f * ((float) ((func_00122BB8() >> 6) & 0xFF) / 255.0f) - 1.55334306f;
        }
        if (*(float *)(arg1 + 0x5C) < 0.0f) {
            if (func_001B2F70(arg0 + 0xB0, D_700038A0) != 0) {
                if ((10.0f + *(float *)0x700038A0 <= *(float *)(arg0 + 0xB4))
                    && (D_00810364 <= *(float *)(arg0 + 0xB4))) {
                } else {
                    *(float *)(arg1 + 0x5C) = func_0011DF78(*(float *)(arg1 + 0x5C));
                }
            }
        } else {
            w = *(volatile float *)(arg1 + 0x5C);
            if (!(w <= 0.0f) && (15.0f + D_00810364) < *(float *)(arg0 + 0xB4)) {
                *(float *)(arg1 + 0x5C) = w * -1.0f;
            }
        }
    }

    *(unsigned short *)(arg1 + 0x2C) = *(unsigned short *)(arg1 + 0x2C) + 1;
    if (flag != 0 || (int) *(unsigned short *)(arg1 + 0x2C) >= 0x12D) {
        *(char *)(arg0 + 5) = 1;
        *(unsigned char *)(arg0 + 6) = 0;
        *(float *)(arg1 + 0x4C) = 0.4f;
        *(float *)(arg1 + 0x44) = 0.4f;
        *(float *)(arg1 + 0x48) = 0.0f;
        *(unsigned short *)(arg1 + 0x2C) = 0;
        *(short *)(arg1 + 0x30) = 0;
        *(short *)(arg1 + 0x2E) = ((func_00122BB8() >> 15) & 0x3F) + 0x78;
        *(unsigned short *)(arg1 + 0x22) = 0;
        *(unsigned short *)(arg1 + 0x20) = 0;
    }

    r = (float) ((func_00122BB8() >> 14) & 0xFF) / 255.0f;
    *(float *)(arg0 + 0xC4) =
        func_001B12B0(*(float *)(arg1 + 0x58), *(float *)(arg0 + 0xC4), 0.0261799395f);
    *(float *)(arg1 + 0x50) =
        func_001B12B0(*(float *)(arg1 + 0x5C), *(float *)(arg1 + 0x50),
                      (3.14159274f * (1.0f + 1.8f * r)) / 180.0f);
    func_0013BBB0(arg0, arg1);
    func_0013BA20(arg0, arg1);
}
