// NEARMISS func_00195130  (vram 0x00195130, 0x1270 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 88.24% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// NEARMISS 88.24 (mwcc233), up from 41.30 at the start of this session. KEY REUSABLE FINDING: mwcc lays out a switch's `default:` body IMMEDIATELY after the compare chain when `default:` is written FIRST in the source. Moving the `default:` arm of the inner `switch(*(int*)(arg1+0x230))` (D_00810700...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// NEARMISS 88.24 (objdiff, mwcc 2.3.3). Logic complete; residual is basic-block
// layout / switch-arm-order permutation in the two deeply nested arms (see wall).
//
// Per-area camera/target driver: arg0 = camera state, arg1 = tracked entity.
// Dispatches on the outer state byte arg0[1], then on the global area id
// D_00810700, then on the entity's sub-state *(int*)(arg1+0x230).
// var_s0 is the "handled" result forwarded to func_00193EB0 at the end.
// NOTE: mwcc places a switch's `default:` body right after the compare chain
// when `default:` is written FIRST - that ordering is load-bearing here.

extern void func_001916C0(void *p, void *e, int a);
extern void func_001921D0();
extern void func_00193D90();
extern void func_00191210();
extern void func_00193EB0(void *p, void *e, int a);
extern int func_001944B0(void *p, void *e, int a);
extern int func_00194D10(void *p, void *e, int a);
extern void func_00194DB0(void *p, void *e, int a);
extern void func_0018D7B0(void *p, int a);
extern int func_0018C4B0(float *d, float a, float b);
extern int func_0018C6A0(void *src, float *d, float f);
extern void func_00191D40(void *p, float a, float b);
extern void func_00192010(void *p, float a, float b, float c);
extern float func_0011DF78(void *p, float f);
extern void func_00102948(void *a, void *b);
extern int func_00230230(void *p, void *e);
extern int func_823FE0(void *p);
extern void func_0022FCA0(void *p, void *e, int a);
extern void func_001AEDE0(int a, int b);
extern float D_008105D0;
extern float D_008105D4;
extern float D_008105D8;
extern float D_0081069C;
extern unsigned char D_008106F2;
extern unsigned char D_00810700;
extern unsigned char D_00810701;
extern unsigned char D_00810702;
extern unsigned char D_00810803;

void func_00195130(unsigned char *arg0, unsigned char *arg1) {
    unsigned char st;
    unsigned char sub;
    int var_s0;
    int v;
    float lim;
    float t;
    float t2;
    float var_f1;
    float d;
    short cnt;

    st = arg0[1];
    var_s0 = 0;
    switch (st) {
    case 0:
        arg0[1] = st + 1;
        arg0[2] = 0;
        arg0[3] = 0;
        *(short *)(arg0 + 8) = 0;
        /* fallthrough */
    case 1:
        func_001916C0(arg0, arg1, 0);
        switch (D_00810700) {
        case 0:
            v = *(int *)(arg1 + 0x230);
            if (v == 0x14 || v == 0x15) {
                *(float *)(arg0 + 0x94) = -10.0f - *(float *)(arg0 + 0xC);
            }
            if (D_00810701 == 2 && D_00810803 == 3) {
                *(float *)(arg0 + 0x94) = 0.0f;
                func_00194DB0(arg0, arg1, 8);
                var_s0 = 2;
            }
            break;
        case 4:
            if (*(char *)(arg0 + 0x6D) != 0) {
                if (!(*(float *)(arg1 + 0xA8) <= 360.0f)) {
                    if (!(*(float *)(arg1 + 0xA4) <= 54.0f)) {
                        *(float *)(arg0 + 0x98) = 0.0f;
                    } else {
                        *(float *)(arg0 + 0x98) = 11.5f;
                    }
                } else {
                    *(float *)(arg0 + 0x98) = 11.5f;
                }
            }
            break;
        case 6:
            if (func_001944B0(arg0, arg1, 0) != 0) {
                var_s0 = 1;
            } else if (func_00194D10(arg0, arg1, 2) != 0) {
                func_0018D7B0(arg0, 5);
                *(float *)(arg0 + 0x14) = 90.0f;
                var_s0 = 1;
                if (func_0018C4B0(&D_008105D0, *(float *)(arg0 + 0x14), 0.7f) != 0) {
                    *(float *)(arg0 + 0x10) = -367.7f;
                    *(float *)(arg0 + 0x18) = -598.4f;
                    func_0018C6A0(arg0 + 0x10, &D_008105D0, 0.7f);
                    var_s0 = 1;
                }
            }
            break;
        case 8:
            if (D_00810701 == 2) {
                if (func_001944B0(arg0, arg1, 8) != 0) {
                    var_s0 = 1;
                }
            } else if (D_00810701 == 3) {
                switch (*(int *)(arg1 + 0x230)) {
                default:
                    if (*(float *)(arg1 + 0xA4) < 217.1f) {
                        if (*(float *)(arg1 + 0xA0) < 145.3f && *(float *)(arg1 + 0xA8) < 163.0f) {
                            *(float *)(arg0 + 0x10) = 131.1f;
                            *(float *)(arg0 + 0x18) = 182.5f;
                        } else if (!(*(float *)(arg1 + 0xA8) < 163.0f)) {
                            if (*(float *)(arg0 + 0x10) != 131.1f) {
                                *(float *)(arg0 + 0x10) = 167.8f;
                                *(float *)(arg0 + 0x18) = 180.9f;
                            } else {
                                *(float *)(arg0 + 0x10) = 131.1f;
                                *(float *)(arg0 + 0x18) = 182.5f;
                            }
                        } else {
                            *(float *)(arg0 + 0x10) = 167.8f;
                            *(float *)(arg0 + 0x18) = 180.9f;
                        }
                        if (*(float *)(arg0 + 0x64) == -46.8f) {
                            lim = -20.0f;
                        } else {
                            lim = -10.0f;
                        }
                        t = D_0081069C - func_0011DF78(arg0, *(float *)(arg0 + 0xC));
                        *(float *)0x70003A20 = t;
                        if (t < lim) {
                            if (lim != -20.0f) {
                                t2 = t - lim;
                                *(float *)0x70003A24 = t2;
                                if (t2 < -10.0f) {
                                    *(float *)0x70003A24 = -10.0f;
                                }
                            } else {
                                *(float *)0x70003A24 = 0.5f * t;
                            }
                            var_f1 = *(float *)(arg0 + 0x8C) + (*(float *)(arg1 + 0xA4) + (*(float *)(arg0 + 0x5C) - *(float *)0x70003A24));
                        } else {
                            var_f1 = *(float *)(arg0 + 0x8C) + (*(float *)(arg0 + 0x5C) + *(float *)(arg1 + 0xA4));
                        }
                        func_00191D40(arg0, 11.0f + var_f1, 4.0f);
                        func_0018D7B0(arg0, 5);
                        func_0018C4B0(&D_008105D0, *(float *)(arg0 + 0x14), 0.8f);
                        var_s0 = 2;
                        D_008105D0 = *(float *)(arg0 + 0x10);
                        D_008105D8 = *(float *)(arg0 + 0x18);
                    }
                    break;
                case 10:
                    if (!(*(float *)(arg1 + 0xA8) <= 150.0f) && !(*(float *)(arg1 + 0xA4) <= 230.0f)) {
                        if (!(*(float *)(arg1 + 0xA8) <= 168.0f)) {
                            *(float *)(arg0 + 0x10) = 131.7f;
                            *(float *)(arg0 + 0x18) = 205.2f;
                            *(float *)(arg0 + 0x14) = 287.2f;
                            D_008105D4 = 287.2f;
                            D_008105D0 = *(float *)(arg0 + 0x10);
                            D_008105D8 = *(float *)(arg0 + 0x18);
                        } else {
                            *(float *)(arg0 + 0x10) = 148.6f;
                            *(float *)(arg0 + 0x18) = 129.2f;
                            if (!(*(float *)(arg0 + 0x14) <= 282.4f)) {
                                *(float *)(arg0 + 0x14) = 282.4f;
                                D_008105D4 = 282.4f;
                            }
                            D_008105D0 = *(float *)(arg0 + 0x10);
                            D_008105D8 = *(float *)(arg0 + 0x18);
                            func_00192010(arg0, *(float *)(arg0 + 0x8C) + (*(float *)(arg0 + 0x5C) + *(float *)(arg1 + 0xB4)), 25.0f, 10.0f);
                        }
                        func_0018D7B0(arg0, 5);
                        func_0018C4B0(&D_008105D0, *(float *)(arg0 + 0x14), 0.8f);
                        var_s0 = 1;
                    }
                    break;
                case 7:
                case 6:
                case 9:
                case 8:
                    d = (*(float *)(arg1 + 0xB0) - 129.7f) * (*(float *)(arg1 + 0xB0) - 129.7f) + (*(float *)(arg1 + 0xB8) - 150.5f) * (*(float *)(arg1 + 0xB8) - 150.5f);
                    if (!(d < 25.0f)) {
                        if (!(*(float *)(arg1 + 0xA8) <= 142.0f) && !(*(float *)(arg1 + 0xA4) <= 230.0f)) {
                            goto far_arm;
                        }
                        d = (*(float *)(arg1 + 0xB0) - 129.8f) * (*(float *)(arg1 + 0xB0) - 129.8f) + (*(float *)(arg1 + 0xB8) - 160.5f) * (*(float *)(arg1 + 0xB8) - 160.5f);
                        if (d < 25.0f) {
                            if (*(float *)(arg0 + 0x14) <= 223.6f) {
                                if (*(float *)(arg0 + 0x14) < 146.2f) {
                                    *(float *)(arg0 + 0x14) = 146.2f;
                                    D_008105D4 = 146.2f;
                                }
                            } else {
                                *(float *)(arg0 + 0x14) = 223.6f;
                                D_008105D4 = 223.6f;
                            }
                            *(float *)(arg0 + 0x10) = 136.7f;
                            *(float *)(arg0 + 0x18) = 182.0f;
                            D_008105D0 = *(float *)(arg0 + 0x10);
                            D_008105D8 = *(float *)(arg0 + 0x18);
                            func_00192010(arg0, *(float *)(arg0 + 0x8C) + (*(float *)(arg0 + 0x5C) + *(float *)(arg1 + 0xB4)), 25.0f, 10.0f);
                            func_0018D7B0(arg0, 5);
                            func_0018C4B0(&D_008105D0, *(float *)(arg0 + 0x14), 0.8f);
                            var_s0 = 1;
                        }
                    } else {
                    far_arm:
                        if (!(*(float *)(arg1 + 0xA4) <= 217.1f)) {
                            *(float *)(arg0 + 0x10) = 148.6f;
                            *(float *)(arg0 + 0x18) = 129.2f;
                            if (*(float *)(arg0 + 0x14) <= 282.4f) {
                                if (*(float *)(arg0 + 0x14) < 240.7f) {
                                    *(float *)(arg0 + 0x14) = 240.7f;
                                    D_008105D4 = 240.7f;
                                }
                            } else {
                                *(float *)(arg0 + 0x14) = 282.4f;
                                D_008105D4 = 282.4f;
                            }
                        } else {
                            *(float *)(arg0 + 0x10) = 136.7f;
                            *(float *)(arg0 + 0x18) = 182.0f;
                            if (*(float *)(arg0 + 0x14) <= 223.6f) {
                                if (*(float *)(arg0 + 0x14) < 146.2f) {
                                    *(float *)(arg0 + 0x14) = 146.2f;
                                    D_008105D4 = 146.2f;
                                }
                            } else {
                                *(float *)(arg0 + 0x14) = 223.6f;
                                D_008105D4 = 223.6f;
                            }
                        }
                        D_008105D0 = *(float *)(arg0 + 0x10);
                        D_008105D8 = *(float *)(arg0 + 0x18);
                        func_00192010(arg0, *(float *)(arg0 + 0x8C) + (*(float *)(arg0 + 0x5C) + *(float *)(arg1 + 0xB4)), 25.0f, 10.0f);
                        func_0018D7B0(arg0, 5);
                        func_0018C4B0(&D_008105D0, *(float *)(arg0 + 0x14), 0.8f);
                        var_s0 = 1;
                    }
                    break;
                }
            }
            break;
        case 0xB:
            if (*(float *)(arg1 + 0xA4) < 185.0f && *(float *)(arg1 + 0xA8) < 220.0f) {
                if (!(*(float *)(arg1 + 0xA0) <= 359.0f) && *(float *)(arg1 + 0xA0) < 394.8f) {
                    func_0022FCA0(arg0, arg1, 8);
                    *(float *)(arg0 + 0x18) = 219.0f;
                    *(float *)(arg0 + 0x14) = 220.0f;
                    func_0018D7B0(arg0, 5);
                    func_0018C6A0(arg0 + 0x10, &D_008105D0, 1.5f);
                    func_0018C4B0(&D_008105D0, *(float *)(arg0 + 0x14), 1.0f);
                    var_s0 = 1;
                    break;
                }
            }
            switch (*(int *)(arg1 + 0x230)) {
            case 7:
            case 6:
            case 9:
            case 8:
                d = (*(float *)(arg1 + 0xA0) - 321.5f) * (*(float *)(arg1 + 0xA0) - 321.5f) + (*(float *)(arg1 + 0xA8) - 216.7f) * (*(float *)(arg1 + 0xA8) - 216.7f);
                if (d < 64.0f) {
                    *(float *)(arg0 + 0x10) = 366.6f;
                    *(float *)(arg0 + 0x18) = 216.1f;
                    func_00192010(arg0, *(float *)(arg0 + 0x8C) + (*(float *)(arg0 + 0x5C) + *(float *)(arg1 + 0xB4)), 25.0f, 20.0f);
                    func_0018D7B0(arg0, 5);
                    if (*(int *)(arg1 + 0x230) == 7) {
                        func_00102948(&D_008105D0, arg0 + 0x10);
                    } else {
                        func_0018C6A0(arg0 + 0x10, &D_008105D0, 0.8f);
                    }
                    func_0018C4B0(&D_008105D0, *(float *)(arg0 + 0x14), 0.8f);
                    var_s0 = 1;
                }
                break;
            }
            break;
        case 0xD:
            if ((int)D_00810702 >= 8) {
                if (func_823FE0(arg0) != 0) {
                    *(float *)(arg0 + 0x10) = 801.3f;
                    *(float *)(arg0 + 0x14) = 282.3f;
                    *(float *)(arg0 + 0x18) = 1171.0f;
                    func_0018D7B0(arg0, 5);
                    func_0018C6A0(arg0 + 0x10, &D_008105D0, 1.2f);
                    func_0018C4B0(&D_008105D0, *(float *)(arg0 + 0x14), 0.7f);
                    var_s0 = 1;
                } else if (*(unsigned char *)(arg1 + 0xF) == 0xB) {
                    arg0[1] = 4;
                    var_s0 = 1;
                    arg0[2] = 0;
                } else if (func_00194D10(arg0, arg1, 0) != 0) {
                    *(float *)(arg0 + 0x10) = 839.8f;
                    *(float *)(arg0 + 0x14) = 198.0f;
                    *(float *)(arg0 + 0x18) = 1217.3f;
                    func_0018D7B0(arg0, 5);
                    func_0018C6A0(arg0 + 0x10, &D_008105D0, 0.7f);
                    func_0018C4B0(&D_008105D0, *(float *)(arg0 + 0x14), 0.7f);
                    var_s0 = 1;
                } else if (*(float *)(arg1 + 0xA0) < 663.0f && *(float *)(arg1 + 0xA8) < 738.5f) {
                    *(float *)(arg0 + 0x10) = 650.9f;
                    *(float *)(arg0 + 0x14) = 187.6f;
                    *(float *)(arg0 + 0x18) = 785.3f;
                    func_0018D7B0(arg0, 5);
                    func_0018C6A0(arg0 + 0x10, &D_008105D0, 1.0f);
                    func_0018C4B0(&D_008105D0, *(float *)(arg0 + 0x14), 1.0f);
                    var_s0 = 1;
                }
            }
            break;
        case 0xE:
            if (func_00230230(arg0, arg1) != 0) {
                var_s0 = 1;
            }
            break;
        case 0xF:
            if (D_00810701 == 1) {
                v = *(int *)(arg1 + 0x230);
                if (v == 0x14 || v == 0x15) {
                    *(float *)(arg0 + 0x94) = -10.0f - *(float *)(arg0 + 0xC);
                }
            }
            break;
        case 0x11:
            d = (340.0f - *(float *)(arg1 + 0xA0)) * (340.0f - *(float *)(arg1 + 0xA0)) + (270.0f - *(float *)(arg1 + 0xA8)) * (270.0f - *(float *)(arg1 + 0xA8));
            if (!(d >= 42025.0f) && d > 19600.0f) {
                *(float *)(arg0 + 0x98) = 20.0f;
            }
            break;
        case 0x13:
            if (D_00810701 == 0) {
                if (func_001944B0(arg0, arg1, D_008106F2) != 0) {
                    var_s0 = 1;
                } else {
                    v = *(int *)(arg1 + 0x230);
                    if (v == 0x14 || v == 0x15) {
                        *(float *)(arg0 + 0x94) = -43.0f - *(float *)(arg0 + 0xC);
                    }
                }
            } else {
                if (func_001944B0(arg0, arg1, 7) != 0) {
                    var_s0 = 1;
                } else {
                    switch (*(int *)(arg1 + 0x230)) {
                    case 7:
                    case 6:
                    case 9:
                    case 8:
                        d = (*(float *)(arg1 + 0xA0) - 892.1f) * (*(float *)(arg1 + 0xA0) - 892.1f) + (*(float *)(arg1 + 0xA8) - 929.5f) * (*(float *)(arg1 + 0xA8) - 929.5f);
                        if (d < 64.0f) {
                            *(float *)(arg0 + 0x10) = 874.8f;
                            *(float *)(arg0 + 0x18) = 887.1f;
                            func_00192010(arg0, *(float *)(arg0 + 0x8C) + (*(float *)(arg0 + 0x5C) + *(float *)(arg1 + 0xB4)), 25.0f, 20.0f);
                            func_0018D7B0(arg0, 5);
                            func_0018C6A0(arg0 + 0x10, &D_008105D0, 0.8f);
                            func_0018C4B0(&D_008105D0, *(float *)(arg0 + 0x14), 0.8f);
                            var_s0 = 1;
                        }
                        break;
                    }
                }
            }
            break;
        }
        if (var_s0 == 0) {
            func_001921D0(arg0, arg1, 0);
        }
        break;
    case 2:
        func_00193D90(arg0, arg1, 1);
        func_0018D7B0(arg0, 0);
        break;
    case 3:
        func_001921D0(arg0, arg1, 1);
        break;
    case 4:
        func_001916C0(arg0, arg1, 0);
        sub = arg0[2];
        switch (sub) {
        case 0:
            *(short *)(arg0 + 8) = 0x78;
            arg0[2] = arg0[2] + 1;
            /* fallthrough */
        case 1:
            cnt = *(short *)(arg0 + 8) - 1;
            *(short *)(arg0 + 8) = cnt;
            if (cnt == 0) {
                arg0[2] = arg0[2] + 1;
            }
            *(float *)(arg0 + 0x10) = 669.3f;
            *(float *)(arg0 + 0x14) = 182.3f;
            *(float *)(arg0 + 0x18) = 1082.5f;
            func_0018C6A0(arg0 + 0x10, &D_008105D0, 1.1f);
            func_0018C4B0(&D_008105D0, *(float *)(arg0 + 0x14), 0.6f);
            break;
        case 2:
            *(float *)(arg0 + 0x10) = 765.6f;
            *(float *)(arg0 + 0x14) = 279.1f;
            *(float *)(arg0 + 0x18) = 1103.4f;
            if ((func_0018C6A0(arg0 + 0x10, &D_008105D0, 0.6f) | func_0018C4B0(&D_008105D0, *(float *)(arg0 + 0x14), 0.48f)) == 7) {
                *(short *)(arg0 + 8) = 0x78;
                arg0[2] = arg0[2] + 1;
            }
            break;
        case 3:
            cnt = *(short *)(arg0 + 8) - 1;
            *(short *)(arg0 + 8) = cnt;
            if (cnt == 0) {
                arg0[2] = arg0[2] + 1;
                func_001AEDE0(4, 0);
            }
            /* fallthrough */
        case 4:
            *(float *)(arg0 + 0x10) = 773.6f;
            *(float *)(arg0 + 0x14) = 387.3f;
            *(float *)(arg0 + 0x18) = 1136.7f;
            func_0018C6A0(arg0 + 0x10, &D_008105D0, 0.6f);
            func_0018C4B0(&D_008105D0, *(float *)(arg0 + 0x14), 0.32f);
            break;
        }
        break;
    }
    func_00191210();
    func_00193EB0(arg0, arg1, var_s0);
}
