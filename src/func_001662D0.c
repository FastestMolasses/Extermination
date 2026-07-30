// NEARMISS func_001662D0  (vram 0x001662D0, 0x19A8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.83% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// 99.83% (mwcc233; 991202 = 88.38%) -- 14 partially-differing instructions out of 1643, ALL pure register-coloring, no structural or scheduling divergence anywhere. Structure fully recovered: the 34-case top-level dispatch is a real `switch` with ASCENDING source cases (CW's descending beq chain), ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

// Ladder / climbable-object actor state machine (one tick).  `self+6` is the
// state byte, `self+7` a per-state sub-step, `self+0x28` a countdown, and
// `self+0x200` the animation event flags (0x1000 = anim finished,
// 0x8000 = anim still blending).  `self+0x24C` selects the ladder variant
// (0/1 = climb up/down, 2/3 = ledge hop) and `self+0x1F1` the mount type.
//
// State groups:
//   0x00-0x04  climb-up cycle (per-step move driven by D_00275B40[0][1])
//   0x0A-0x0C  climb-down cycle
//   0x14-0x16  mount at the top
//   0x1E-0x20  mount at the bottom
//   0x28-0x2D  dismount / jump-off
//   0x32-0x35  ledge hop variant 0
//   0x3C-0x3F  ledge hop variant 1
//   0x46-0x48  8-frame interpolated hop to self+0x2F4/0x2F8/0x258
//   0x50-0x52  12-frame interpolated warp to self+0x290..0x298
// Every state that keeps the actor alive falls through to the shared tail
// which re-runs func_00176DC0 in camera mode 2.
extern short float_to_int(float);
extern void func_001026A0(void *dst, void *m, void *v);
extern void func_00102948(void *dst, void *src);
extern void func_001031E0(void *dst, void *src);
extern float func_0011DF78(float);
extern void func_001749A0(unsigned char *, int, int, float);
extern void func_00174AB0(unsigned char *);
extern void func_00174AC0(unsigned char *, int);
extern void func_00174FD0(unsigned char *);
extern int func_00175900(unsigned char *, int);
extern void func_00176DC0(unsigned char *);
extern void func_00178B90(unsigned char *, int);
extern void func_0017C440(unsigned char *, int);
extern void func_0017C540(unsigned char *);
extern void func_0017FC80(unsigned char *, float);
extern void func_0017FD00(unsigned char *, float);
extern void func_0017FD40(unsigned char *, float);
extern void func_0017FD80(unsigned char *, int, float);
extern void func_0017FE00(unsigned char *, int, float);
extern void func_0017FE80(unsigned char *, int, float);
extern void func_0017FF00(unsigned char *, int, float);
extern int func_00180300(unsigned char *, void *, int);
extern int func_00180460(unsigned char *, int);
extern int func_00180530(unsigned char *, int);
extern int func_00180600(unsigned char *, int, float, float, float);
extern unsigned char func_001809B0(unsigned char *, int);
extern int func_00181110(unsigned char *, int);
extern void func_00182430(unsigned char *, int);
extern void func_00182A70(unsigned char *);
extern void func_00187EE0(unsigned char *, void *, void *);
extern int func_00188550(unsigned char *, unsigned char);
extern int func_00199FA0(void *, void *);
extern float func_001B12B0(float, float, float);
extern float func_001B1470(float);
extern void func_001B61C0(int, int, int, int);
extern int func_001C61D0(int, short);
extern void func_001C68C0(int);
extern void func_001FB9F0(int, int, int, int);
extern void func_001FBD50(unsigned char *, int, int, float);
extern void func_0021C270(unsigned char *);
extern void func_0021C350(unsigned char *);

extern short D_002754D0[16];
extern char **D_00275B40;
extern signed char D_008106F2[16];
extern unsigned char D_00810700[16];
extern unsigned short D_00810E74[16];
extern char D_700038A0[];
extern char D_700038B0[];

void func_001662D0(unsigned char *self)
{
    float pa[4];
    float pb[4];
    float ang;
    float d;
    int r;
    int flg;
    int step;
    unsigned char st;
    unsigned char sub;
    short cnt;

    st = *(unsigned char *)(self + 6);
    switch (st) {
    case 0:
        *(unsigned char *)(self + 6) = st + 1;
        *(unsigned char *)(self + 7) = 0;
        func_0017FC80(self, 16.0f);
        func_001031E0(self + 0x290, self + 0xB0);
        /* fallthrough */
    case 1:
        if (func_00181110(self, 0) != 0) {
            return;
        }
        func_00174FD0(self);
        step = *(int *)(self + 0x24C);
        if (step == 0) {
            r = func_00180460(self, 1);
            if (r == 0) {
                *(unsigned char *)(self + 6) = *(unsigned char *)(self + 6) + 1;
            } else if (r == 2 && *(unsigned char *)(self + 0xD) == 0) {
                *(unsigned char *)(self + 6) = 0x14;
            }
        } else if (step == 1) {
            r = func_00180530(self, 1);
            if (r == 0) {
                *(unsigned char *)(self + 6) = 0xA;
            } else if (r == 1) {
                *(unsigned char *)(self + 6) = 0x28;
            } else {
                *(unsigned char *)(self + 6) = 0x1E;
            }
        } else if (step == 2) {
            if (func_00180600(self, 0, 9.0f, 10.0f, -2.0f) == 0) {
                *(unsigned char *)(self + 6) = 0x32;
            }
        } else if (step == 3) {
            if (func_00180600(self, 1, 9.0f, 10.0f, -2.0f) == 0) {
                *(unsigned char *)(self + 6) = 0x3C;
            }
        }
        *(float *)(self + 0x21C) = 0.0f;
        *(float *)(self + 0x38) = 0.0f;
        break;
    case 2:
        if (func_00181110(self, 0) != 0) {
            return;
        }
        *(unsigned char *)(self + 6) = *(unsigned char *)(self + 6) + 1;
        func_0017FD00(self, 4.0f);
        break;
    case 3:
        if (func_00181110(self, 0) != 0) {
            return;
        }
        if (!(*(int *)(self + 0x200) & 0x8000)) {
            *(unsigned char *)(self + 6) = *(unsigned char *)(self + 6) + 1;
        }
        break;
    case 4:
        func_00174FD0(self);
        if (*(int *)(self + 0x200) & 0x1000) {
            *(float *)(self + 0xB4) = 3.0f + *(float *)(self + 0x294);
            func_00182A70(self);
            func_001FB9F0(0x107, 0x1000, 0x1000, 0x1000);
            *(unsigned char *)(self + 0x2F1) = 1 - *(unsigned char *)(self + 0x2F1);
            if (*(int *)(self + 0x24C) == 0) {
                r = func_00180460(self, 1);
                if (r == 0) {
                    func_0017FD00(self, 1.0f);
                    *(float *)(self + 0x21C) = 0.0f;
                } else if (r == 2 && *(unsigned char *)(self + 0xD) == 0) {
                    *(unsigned char *)(self + 6) = 0x14;
                } else {
                    *(unsigned char *)(self + 6) = 1;
                    func_0017FC80(self, 16.0f);
                }
            } else {
                *(unsigned char *)(self + 6) = 1;
                func_0017FC80(self, 16.0f);
            }
            func_001031E0(self + 0x290, self + 0xB0);
        } else {
            *(float *)(self + 0x38) = *(float *)(D_00275B40[0] + 4) - *(float *)(self + 0x21C);
            *(float *)(self + 0x21C) = *(float *)(D_00275B40[0] + 4);
            *(float *)(self + 0xB4) += *(float *)(self + 0x38);
            if (*(int *)(self + 0x24C) == 0) {
                if (*(unsigned char *)(self + 0x23F) == 3) {
                    *(float *)(self + 0x204) = 2.0f;
                } else if (*(unsigned char *)(self + 0x23F) == 2) {
                    *(float *)(self + 0x204) = 1.5f;
                } else {
                    *(float *)(self + 0x204) = 1.0f;
                }
            }
        }
        if (func_00181110(self, 1) != 0) {
            return;
        }
        break;
    case 0xA:
        if (func_00181110(self, 0) != 0) {
            return;
        }
        *(unsigned char *)(self + 6) = *(unsigned char *)(self + 6) + 1;
        func_0017FD40(self, 4.0f);
        break;
    case 0xB:
        if (func_00181110(self, 0) != 0) {
            return;
        }
        if (!(*(int *)(self + 0x200) & 0x8000)) {
            *(unsigned char *)(self + 6) = *(unsigned char *)(self + 6) + 1;
        }
        break;
    case 0xC:
        func_00174FD0(self);
        if (*(int *)(self + 0x200) & 0x1000) {
            *(float *)(self + 0xB4) = *(float *)(self + 0x294) - 3.0f;
            func_00182A70(self);
            func_001FB9F0(0x107, 0x1000, 0x1000, 0x1000);
            *(unsigned char *)(self + 0x2F1) = 1 - *(unsigned char *)(self + 0x2F1);
            if (*(int *)(self + 0x24C) == 1) {
                r = func_00180530(self, 1);
                if (r == 0) {
                    func_0017FD40(self, 0.0f);
                    *(float *)(self + 0x21C) = 0.0f;
                } else if (r == 1) {
                    *(unsigned char *)(self + 6) = 0x28;
                } else {
                    *(unsigned char *)(self + 6) = 0x1E;
                }
            } else {
                *(unsigned char *)(self + 6) = 1;
                func_0017FC80(self, 16.0f);
            }
            func_001031E0(self + 0x290, self + 0xB0);
        } else {
            *(float *)(self + 0x38) = *(float *)(D_00275B40[0] + 4) - *(float *)(self + 0x21C);
            *(float *)(self + 0x21C) = *(float *)(D_00275B40[0] + 4);
            *(float *)(self + 0xB4) += *(float *)(self + 0x38);
            if (*(int *)(self + 0x24C) == 1) {
                if (*(unsigned char *)(self + 0x23F) == 3) {
                    *(float *)(self + 0x204) = 2.0f;
                } else if (*(unsigned char *)(self + 0x23F) == 2) {
                    *(float *)(self + 0x204) = 1.5f;
                } else {
                    *(float *)(self + 0x204) = 1.0f;
                }
            }
        }
        if (func_00181110(self, 1) != 0) {
            return;
        }
        break;
    case 0x14:
        *(unsigned char *)(self + 6) = st + 1;
        *(unsigned char *)(self + 7) = 0;
        *(signed char *)(self + 0x1F0) = 0x18;
        *(short *)(self + 0x28) = 0;
        func_001749A0(self, 0xF0, 1, 4.0f);
        break;
    case 0x15:
        flg = *(int *)(self + 0x200);
        if (flg & 0x1000) {
            func_001C68C0(flg);
            func_00102948(self + 0xB0, D_00275B40[1] + 0xC0);
            *(float *)(self + 0xB4) = *(float *)(self + 0xB4) - 11.5f;
            *(float *)(self + 0xB4) += -0.4f;
            if (func_00175900(self, 1) != 0) {
                func_00182430(self, 2);
                func_00187EE0(self, self + 0xB0, self + 0xD0);
            }
            func_00174AB0(self);
            func_00174AC0(self, 0);
            if ((int)*(unsigned char *)(self + 0x23F) >= 2) {
                *(unsigned char *)(self + 6) = *(unsigned char *)(self + 6) + 1;
                func_0017C440(self, 1);
            } else {
                *(signed char *)(self + 0x25C) = 0;
                func_0017C540(self);
            }
        } else if (!(flg & 0x8000)) {
            sub = *(unsigned char *)(self + 7);
            switch (sub) {
            case 0:
                func_001031E0(D_700038A0, self + 0xB0);
                *(volatile float *)0x700038A4 = *(volatile float *)0x700038A4 + 10.0f;
                if (func_00180300(self, D_700038A0, 0) == 0 && func_00199FA0(pa, pb) != 0) {
                    *(volatile float *)0x70003A20 = func_0011DF78(pb[1] - *(float *)(self + 0xB4));
                    *(volatile float *)0x70003A20 = *(volatile float *)0x70003A20 - 16.8f;
                    *(float *)(self + 0x2E4) = *(volatile float *)0x70003A20
                        / (float)func_001C61D0(*(int *)(self + 0x40), *(short *)(self + 0x20C));
                    *(unsigned char *)(self + 7) = *(unsigned char *)(self + 7) + 1;
                }
                break;
            case 1:
                if (*(float *)(self + 0x3C) <= 1.0f) {
                    *(unsigned char *)(self + 7) = sub + 1;
                }
                *(float *)(self + 0xB4) += *(float *)(self + 0x2E4);
                break;
            case 2:
                break;
            }
            cnt = *(short *)(self + 0x28);
            switch (cnt) {
            case 0:
                if (*(float *)(self + 0x3C) <= 78.0f) {
                    *(short *)(self + 0x28) = cnt + 1;
                    func_00182A70(self);
                }
                break;
            case 1:
                if (*(float *)(self + 0x3C) <= 64.0f) {
                    *(short *)(self + 0x28) = cnt + 1;
                    func_00182A70(self);
                }
                break;
            case 2:
                if (*(float *)(self + 0x3C) <= 48.0f) {
                    *(short *)(self + 0x28) = cnt + 1;
                    func_00182A70(self);
                }
                break;
            case 3:
                if (*(float *)(self + 0x3C) <= 30.0f) {
                    *(short *)(self + 0x28) = cnt + 1;
                    func_00182A70(self);
                }
                break;
            }
        }
        break;
    case 0x16:
        func_00174AC0(self, 0);
        func_00178B90(self, 1);
        if (!(*(int *)(self + 0x200) & 0x8000)) {
            func_0017C540(self);
        }
        break;
    case 0x1E:
        *(unsigned char *)(self + 6) = st + 1;
        *(signed char *)(self + 0x1F0) = 0x18;
        *(short *)(self + 0x28) = 0;
        func_001749A0(self, 0xF1, 1, 8.0f);
        break;
    case 0x1F:
        if (!(*(int *)(self + 0x200) & 0x8000)) {
            *(unsigned char *)(self + 6) = st + 1;
        }
        break;
    case 0x20:
        if (*(int *)(self + 0x200) & 0x1000) {
            func_001C68C0(0x20);
            *(float *)(self + 0xB0) = *(float *)(D_00275B40[1] + 0xC0);
            *(float *)(self + 0xB4) = *(float *)(D_00275B40[1] + 0xC4) - 11.5f;
            *(float *)(self + 0xB8) = *(float *)(D_00275B40[1] + 0xC8);
            *(float *)(self + 0xB4) += -0.2f;
            func_00175900(self, 1);
            func_00174AB0(self);
            *(signed char *)(self + 4) = 1;
            *(signed char *)(self + 5) = 0;
            *(unsigned char *)(self + 6) = 0;
            *(signed char *)(self + 0x1F0) = 0;
        }
        cnt = *(short *)(self + 0x28);
        switch (cnt) {
        case 0:
            if (*(float *)(self + 0x3C) <= 22.0f) {
                *(short *)(self + 0x28) = cnt + 1;
                func_001031E0(self + 0x290, self + 0xB0);
                func_001C68C0((int)self);
                *(float *)(self + 0xB0) = *(float *)(D_00275B40[1] + 0xC0);
                *(float *)(self + 0xB4) = *(float *)(D_00275B40[1] + 0xC4) - 11.5f;
                *(float *)(self + 0xB8) = *(float *)(D_00275B40[1] + 0xC8);
                *(float *)(self + 0xB4) += -0.4f;
                if (func_00175900(self, 1) != 0) {
                    func_00182430(self, 2);
                    func_00187EE0(self, self + 0xB0, self + 0xD0);
                }
                func_001031E0(self + 0xB0, self + 0x290);
            }
            break;
        case 1:
            break;
        }
        break;
    case 0x28:
        *(unsigned char *)(self + 6) = st + 1;
        *(signed char *)(self + 0x1F0) = 0x1A;
        func_001749A0(self, 0xFF, 1, 4.0f);
        break;
    case 0x29:
        if ((*(int *)(self + 0x200) & 0x8000) == 0) {
            *(unsigned char *)(self + 6) = st + 1;
            func_00182A70(self);
        }
        break;
    case 0x2A:
        if (*(int *)(self + 0x200) & 0x1000) {
            *(unsigned char *)(self + 6) = st + 1;
        }
        break;
    case 0x2B:
        if (*(float *)(self + 0x224) != 0.0f) {
            func_001B61C0(0, 0xC0, 5, 1);
            func_001FBD50(self, 0x152, 0, 300.0f);
            func_0021C350(self);
            *(int *)(self + 0x24C) = 1;
        } else if (*(float *)(self + 0x22C) != 0.0f) {
            func_001B61C0(0, 0xC0, 5, 1);
            func_001FBD50(self, 0x153, 0, 300.0f);
            func_0021C270(self);
            *(int *)(self + 0x24C) = 1;
        } else {
            func_00174FD0(self);
        }
        if (*(int *)(self + 0x24C) == 0) {
            *(unsigned char *)(self + 6) = *(unsigned char *)(self + 6) + 1;
            *(short *)(self + 0x28) = 0;
            func_001749A0(self, 0xFE, 1, 1.0f);
        } else if (*(int *)(self + 0x24C) == 1) {
            *(unsigned char *)(self + 6) = *(unsigned char *)(self + 6) + 2;
            *(float *)(self + 0xB0) = *(float *)(D_00275B40[1] + 0xC0);
            *(float *)(self + 0xB8) = *(float *)(D_00275B40[1] + 0xC8);
            *(float *)(self + 0xB4) = *(float *)(self + 0xB4) - 7.2f;
            func_001749A0(self, 0x80, 1, 0.0f);
            *(float *)(self + 0x21C) = 0.0f;
            *(float *)(self + 0x38) = 0.0f;
            *(float *)(self + 0x2E4) = 0.0f;
            *(float *)(self + 0x2F4) = *(float *)(self + 0xB4);
        }
        break;
    case 0x2C:
        if (*(int *)(self + 0x200) & 0x1000) {
            *(unsigned char *)(self + 6) = 1;
            *(float *)(self + 0xB4) = *(float *)(D_00275B40[1] + 0xC4) - 10.5f;
            *(unsigned char *)(self + 0x2F1) = 0;
            func_001749A0(self, D_002754D0[0], 0, 0.0f);
            func_0017FC80(self, 16.0f);
            *(signed char *)(self + 0x1F0) = 0x17;
        }
        cnt = *(short *)(self + 0x28);
        switch (cnt) {
        case 0:
            if (*(float *)(self + 0x3C) <= 20.0f) {
                func_00182A70(self);
                *(short *)(self + 0x28) = *(short *)(self + 0x28) + 1;
            }
            break;
        case 1:
            if (*(float *)(self + 0x3C) <= 2.0f) {
                func_00182A70(self);
                *(short *)(self + 0x28) = *(short *)(self + 0x28) + 1;
            }
            break;
        case 2:
            break;
        }
        break;
    case 0x2D:
        if (*(int *)(self + 0x200) & 0x1000) {
            *(signed char *)(self + 5) = 7;
            *(unsigned char *)(self + 6) = 0;
            *(signed char *)(self + 0x1F0) = 0xD;
        } else {
            *(float *)(self + 0x38) = *(float *)(D_00275B40[0] + 8) - *(float *)(self + 0x21C);
            *(float *)(self + 0x21C) = *(float *)(D_00275B40[0] + 8);
            func_00178B90(self, 1);
            *(float *)(self + 0x2EC) = *(float *)(D_00275B40[0] + 4) - *(float *)(self + 0x2E4);
            *(float *)(self + 0x2E4) = *(float *)(D_00275B40[0] + 4);
            *(float *)(self + 0xB4) += *(float *)(self + 0x2EC);
        }
        break;
    case 0x32:
        if (func_00181110(self, 0) != 0) {
            return;
        }
        *(unsigned char *)(self + 6) = *(unsigned char *)(self + 6) + 1;
        func_001FB9F0(0x10E, 0x1000, 0x1000, 0x1000);
        func_0017FD80(self, 0, 4.0f);
        break;
    case 0x33:
        if (func_00181110(self, 0) != 0) {
            return;
        }
        if (*(int *)(self + 0x200) & 0x1000) {
            *(unsigned char *)(self + 6) = *(unsigned char *)(self + 6) + 1;
            func_0017FE00(self, 0, 1.0f);
        } else {
            *(float *)(self + 0x38) = *(float *)D_00275B40[0] - *(float *)(self + 0x21C);
            *(float *)(self + 0x21C) = *(float *)D_00275B40[0];
            *(volatile float *)0x700038A0 = *(float *)(self + 0x38);
            *(volatile float *)0x700038A4 = 0.0f;
            *(volatile float *)0x700038A8 = 0.0f;
            *(volatile float *)0x700038AC = 0.0f;
            func_001026A0(D_700038B0, self + 0xD0, D_700038A0);
            *(float *)(self + 0xB0) += *(volatile float *)0x700038B0;
            *(float *)(self + 0xB8) += *(volatile float *)0x700038B8;
        }
        break;
    case 0x34:
        if (func_00181110(self, 0) != 0) {
            return;
        }
        if (D_00810E74[0] & *(volatile unsigned short *)0x70003B76) {
            *(unsigned char *)(self + 0x1F1) = func_001809B0(self, 0);
            if (*(unsigned char *)(self + 0x1F1) == 7) {
                *(unsigned char *)(self + 6) = 0x50;
                *(signed char *)(self + 0x1F0) = 0x19;
                func_0017FF00(self, 0, 1.0f);
                D_008106F2[0] = 5;
            } else if (*(unsigned char *)(self + 0x1F1) != 0) {
                *(unsigned char *)(self + 6) = 0x46;
                *(signed char *)(self + 0x1F0) = 0x19;
                func_0017FF00(self, 0, 1.0f);
            }
        } else {
            func_00174FD0(self);
            if (*(int *)(self + 0x24C) != 2) {
                *(unsigned char *)(self + 6) = *(unsigned char *)(self + 6) + 1;
                func_0017FE80(self, 0, 1.0f);
            }
        }
        break;
    case 0x35:
        if (func_00181110(self, 0) != 0) {
            return;
        }
        if (*(int *)(self + 0x200) & 0x1000) {
            *(unsigned char *)(self + 6) = 1;
            func_0017FC80(self, 16.0f);
        }
        break;
    case 0x3C:
        if (func_00181110(self, 0) != 0) {
            return;
        }
        *(unsigned char *)(self + 6) = *(unsigned char *)(self + 6) + 1;
        func_001FB9F0(0x10F, 0x1000, 0x1000, 0x1000);
        func_0017FD80(self, 1, 4.0f);
        break;
    case 0x3D:
        if (func_00181110(self, 0) != 0) {
            return;
        }
        if (*(int *)(self + 0x200) & 0x1000) {
            *(unsigned char *)(self + 6) = *(unsigned char *)(self + 6) + 1;
            func_0017FE00(self, 1, 1.0f);
        } else {
            *(float *)(self + 0x38) = *(float *)D_00275B40[0] - *(float *)(self + 0x21C);
            *(float *)(self + 0x21C) = *(float *)D_00275B40[0];
            *(volatile float *)0x700038A0 = *(float *)(self + 0x38);
            *(volatile float *)0x700038A4 = 0.0f;
            *(volatile float *)0x700038A8 = 0.0f;
            *(volatile float *)0x700038AC = 0.0f;
            func_001026A0(D_700038B0, self + 0xD0, D_700038A0);
            *(float *)(self + 0xB0) += *(volatile float *)0x700038B0;
            *(float *)(self + 0xB8) += *(volatile float *)0x700038B8;
        }
        break;
    case 0x3E:
        if (func_00181110(self, 0) != 0) {
            return;
        }
        if (D_00810E74[0] & *(volatile unsigned short *)0x70003B76) {
            *(unsigned char *)(self + 0x1F1) = func_001809B0(self, 1);
            if (*(unsigned char *)(self + 0x1F1) == 7) {
                *(unsigned char *)(self + 6) = 0x50;
                *(signed char *)(self + 0x1F0) = 0x19;
                func_0017FF00(self, 1, 1.0f);
                D_008106F2[0] = 4;
            } else if (*(unsigned char *)(self + 0x1F1) != 0) {
                *(unsigned char *)(self + 6) = 0x46;
                *(signed char *)(self + 0x1F0) = 0x19;
                func_0017FF00(self, 1, 1.0f);
            }
        } else {
            func_00174FD0(self);
            if (*(int *)(self + 0x24C) != 3) {
                *(unsigned char *)(self + 6) = *(unsigned char *)(self + 6) + 1;
                func_0017FE80(self, 1, 1.0f);
            }
        }
        break;
    case 0x3F:
        if (func_00181110(self, 0) != 0) {
            return;
        }
        if (*(int *)(self + 0x200) & 0x1000) {
            *(unsigned char *)(self + 6) = 1;
            func_0017FC80(self, 16.0f);
        }
        break;
    case 0x46:
        if (*(float *)(self + 0x3C) <= 6.0f) {
            *(unsigned char *)(self + 6) = st + 1;
            func_001FBD50(self, 0x187, 0, 300.0f);
            *(float *)(self + 0x2F4) = *(float *)(self + 0x2E0);
            *(float *)(self + 0x2F8) = *(float *)(self + 0x2E8);
            *(float *)(self + 0x258) = *(float *)(self + 0x2E4);
            *(short *)(self + 0x28) = 8;
            *(float *)(self + 0x2E0) = (*(float *)(self + 0x2F4) - *(float *)(self + 0xB0)) / 8.0f;
            *(float *)(self + 0x2E8) = (*(float *)(self + 0x2F8) - *(float *)(self + 0xB8)) / 8.0f;
            *(float *)(self + 0x2E4) = (*(float *)(self + 0x258) - *(float *)(self + 0xB4)) / 8.0f;
            ang = func_001B1470(*(float *)(self + 0x218) - *(float *)(self + 0xC4));
            *(volatile float *)0x70003A20 = ang;
            if (!(ang < 0.0f)) {
                *(float *)(self + 0x26C) = ang / 8.0f;
            } else {
                *(float *)(self + 0x26C) = -ang / 8.0f;
            }
            if (*(unsigned char *)(self + 0x1F1) == 1) {
                func_001749A0(self, 0xE5, 0, 8.0f);
            } else {
                func_001749A0(self, 0x7A, 0, 8.0f);
            }
        }
        break;
    case 0x47:
        if (*(short *)(self + 0x28) == 0) {
            *(unsigned char *)(self + 6) = st + 1;
            *(float *)(self + 0xB0) = *(float *)(self + 0x2F4);
            *(float *)(self + 0xB8) = *(float *)(self + 0x2F8);
            *(float *)(self + 0xB4) = *(float *)(self + 0x258);
            *(float *)(self + 0xC4) = *(float *)(self + 0x218);
            if (*(unsigned char *)(self + 0x1F1) == 1) {
                func_00182A70(self);
            } else {
                func_001FBD50(self, 0xFF, 0, 300.0f);
            }
        } else {
            *(float *)(self + 0xB0) += *(float *)(self + 0x2E0);
            *(float *)(self + 0xB8) += *(float *)(self + 0x2E8);
            *(float *)(self + 0xB4) += *(float *)(self + 0x2E4);
            *(float *)(self + 0xC4) = func_001B12B0(*(float *)(self + 0x218),
                                                    *(float *)(self + 0xC4),
                                                    *(float *)(self + 0x26C));
            *(short *)(self + 0x28) = *(short *)(self + 0x28) - 1;
        }
        break;
    case 0x48:
        if (*(int *)(self + 0x200) & 0x1000) {
            sub = *(unsigned char *)(self + 0x1F1);
            if (sub == 1) {
                func_00182A70(self);
                *(unsigned char *)(self + 6) = 0;
                *(signed char *)(self + 0x1F0) = 0x17;
                *(unsigned char *)(self + 0x2F1) = 0;
                func_0017FC80(self, 16.0f);
            } else {
                if (sub == 5) {
                    *(signed char *)(self + 5) = 0x18;
                    *(unsigned char *)(self + 6) = 0;
                    *(signed char *)(self + 0x1F0) = 0x2C;
                    *(unsigned char *)(self + 0x1F1) = 0;
                    *(unsigned char *)(self + 0xD) = 2;
                } else if (sub == 3) {
                    *(signed char *)(self + 5) = 9;
                    *(unsigned char *)(self + 6) = 0;
                    *(signed char *)(self + 0x1F0) = 0x10;
                    *(unsigned char *)(self + 0xD) = 1;
                } else {
                    *(signed char *)(self + 5) = 9;
                    *(unsigned char *)(self + 6) = 0;
                    *(signed char *)(self + 0x1F0) = 0x10;
                    *(unsigned char *)(self + 0xD) = 0;
                }
                func_001749A0(self, func_00188550(self, sub), 0, 16.0f);
            }
        }
        break;
    case 0x50:
        if (*(int *)(self + 0x200) & 0x1000) {
            *(unsigned char *)(self + 6) = st + 1;
            func_001FBD50(self, 0x187, 0, 300.0f);
            *(volatile float *)0x70003A20 = 12.0f;
            func_001749A0(self, 0xBA, 0, 12.0f);
            *(short *)(self + 0x28) = float_to_int(*(volatile float *)0x70003A20);
            *(float *)(self + 0x260) = (*(float *)(self + 0x290) - *(float *)(self + 0xB0))
                / *(volatile float *)0x70003A20;
            *(float *)(self + 0x264) = (*(float *)(self + 0x298) - *(float *)(self + 0xB8))
                / *(volatile float *)0x70003A20;
            *(float *)(self + 0x258) = (*(float *)(self + 0x294) - *(float *)(self + 0xB4))
                / *(volatile float *)0x70003A20;
            ang = func_001B1470(*(float *)(self + 0x218) - *(float *)(self + 0xC4));
            *(volatile float *)0x70003A24 = ang;
            if (!(ang < 0.0f)) {
                *(float *)(self + 0x26C) = ang / *(volatile float *)0x70003A20;
            } else {
                *(float *)(self + 0x26C) = -ang / *(volatile float *)0x70003A20;
            }
        }
        break;
    case 0x51:
        if (*(short *)(self + 0x28) == 0) {
            *(unsigned char *)(self + 6) = st + 1;
            func_001FBD50(self, 0x123, 0, 300.0f);
            *(float *)(self + 0xB0) = *(float *)(self + 0x290);
            *(float *)(self + 0xB4) = *(float *)(self + 0x294);
            *(float *)(self + 0xB8) = *(float *)(self + 0x298);
            *(float *)(self + 0xC4) = *(float *)(self + 0x218);
        } else {
            *(float *)(self + 0xB0) += *(float *)(self + 0x260);
            *(float *)(self + 0xB8) += *(float *)(self + 0x264);
            *(float *)(self + 0xB4) += *(float *)(self + 0x258);
            *(float *)(self + 0xC4) = func_001B12B0(*(float *)(self + 0x218),
                                                    *(float *)(self + 0xC4),
                                                    *(float *)(self + 0x26C));
            *(short *)(self + 0x28) = *(short *)(self + 0x28) - 1;
        }
        break;
    case 0x52:
        if (*(int *)(self + 0x200) & 0x1000) {
            *(signed char *)(self + 5) = 0x10;
            *(unsigned char *)(self + 6) = 0;
            *(signed char *)(self + 0x1F0) = 0x21;
            *(unsigned char *)(self + 0x2F1) = 0;
        }
        break;
    }
    if (D_00810700[0] == 2) {
        func_00176DC0(self);
    }
}
