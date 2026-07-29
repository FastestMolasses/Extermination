// NEARMISS func_001647D0  (vram 0x001647D0, 0x138C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.84% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// NEARMISS 98.84 (mwcc233). Logic fully recovered; all 1251 instructions structurally reproduced. 6 residual artifact clusters (~15 insns total), all register-coloring / delay-slot-scheduling permutations: (1) case 0 scratchpad: target store-forwards the 0x700038A28 value (emits a redundant `mov.s ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

// NEARMISS 98.84 (objdiff, mwcc 2.3.3). Body/structure fully recovered; residual
// is register-coloring + delay-slot-scheduling permutation in case 0x21 and a few
// branch-likely / store-forwarding artifacts. See docs/NEARMISS.md.
//
// Player/entity state machine: dispatches on the state byte at arg0[6], with a
// sub-state at arg0[7]. Scratchpad addresses 0x7000xxxx are EE SPR temporaries;
// D_700038A0/B0/C0/D0 are the same SPR block referenced by symbol when its
// address is passed to a function.

extern int func_0017F240(void *p, int a);
extern int func_0017F320(void *p);
extern void func_00174FD0(void *p);
extern int func_0017E250(void *p, void *v);
extern int func_0017E510(void *p);
extern int func_0017E7C0(void *p, int side);
extern void func_001749A0(void *p, int clip, int flags, float blend);
extern void func_001FBD50(void *p, int id, int flags, float vol);
extern void func_001C68C0(void *p);
extern void func_00178B90(void *p, int a);
extern int func_00175900(void *p, int a);
extern void func_001026A0(void *dst, void *m, void *src);
extern void func_001028B8(void *dst, void *a, void *b);
extern int func_001760C0(void *p, void *v, int a, float f);
extern void func_00182870(void *p, int a);
extern void func_00174AC0(void *p, int a);
extern void func_0017C440(void *p, int a);
extern void func_0017C540(void *p);
extern void func_001796C0(void *p);
extern void func_0017DF70(void *p, int side, float f);
extern void func_0017DFB0(void *p, int side, float f);
extern void func_0017E0D0(void *p, int side, float f);
extern void func_0017E150(void *p, int side, float f);
extern void func_0017E1D0(void *p, int side, float f);
extern void func_00182250(void *p);
extern void func_00182AF0(void *p);
extern void func_00182A70(void *p);
extern void func_0017FC80(void *p, float f);
extern void func_0017FF80(void *p, float f);
extern int func_00188550(void *p);
extern int func_00178910(void *p, int a);
extern int func_0019AFE0(void *p, void *a, void *b, int mask);
extern float func_0011DE90(float a);
extern float func_0011E2A8(float a);
extern float func_0011E748(float a);
extern float func_001B1470(float a);
extern float func_001B12B0(float a, float b, float c);

extern char *D_00275B40;
extern int D_00275498[2];
extern unsigned char D_00810700[];
extern unsigned short D_00810E74[];
extern float D_002485E0[];
extern float D_002485F0[];
extern float D_00248600[];
extern char D_700038A0[];
extern char D_700038B0[];
extern char D_700038C0[];
extern char D_700038D0[];

void func_001647D0(unsigned char *arg0) {
    unsigned char st;
    unsigned char sub;
    int mode;
    int r;
    int flags;
    float f;
    float t;

    st = arg0[6];
    switch (st) {
    case 0:
        arg0[6] = st + 1;
        arg0[0x23C] = 0;
        arg0[0x23D] = 0;
        arg0[0x23E] = 0;
        arg0[0x2F2] = 0;
        arg0[0x2F1] = 0;
        *(float *)(arg0 + 0x38) = 0.0f;
        func_00182250(arg0);
        arg0[0x316] = 0;
        if (D_00810700[0] == 0x11 && arg0[0xD] == 0) {
            *(float *)0x70003A20 = *(float *)(arg0 + 0xB0) - 340.0f;
            t = *(float *)(arg0 + 0xB8) - 270.0f;
            f = *(float *)0x70003A20;
            *(float *)0x70003A28 = t;
            f = func_0011E748(f * f + t * t);
            *(float *)0x70003A2C = f;
            if (f <= 115.0f) {
                arg0[0x316] = 1;
            }
        }
        /* fallthrough */
    case 1:
        if (func_0017F240(arg0, 0) != 0) {
            break;
        }
        if (func_0017F320(arg0) != 0) {
            arg0[6] = 0xA;
            arg0[0x1F0] = 0x13;
            break;
        }
        if (*(unsigned char *)0x70003B8D != 0) {
            *(int *)(arg0 + 0x24C) = 0;
            arg0[0x23F] = 2;
        } else {
            func_00174FD0(arg0);
        }
        mode = *(int *)(arg0 + 0x24C);
        if (mode == 0) {
            if (arg0[0xD] != 1) {
                if (func_0017E250(arg0, arg0 + 0xB0) == 0 && func_0017E510(arg0) == 0) {
                    arg0[6] = arg0[6] + 1;
                    arg0[0x1F0] = 0x11;
                    *(float *)(arg0 + 0x26C) = D_00248600[arg0[0x23F]];
                    func_001749A0(arg0, 0x7C, 0, 5.0f);
                } else if (*(unsigned char *)0x70003B8D != 0) {
                    arg0[6] = 0xA;
                    arg0[0x1F0] = 0x13;
                }
            }
        } else if (mode == 1) {
            if (D_00810E74[0] & *(unsigned short *)0x70003B76) {
                arg0[6] = 0xA;
                arg0[0x1F0] = 0x13;
            }
        } else if (mode == 2) {
            arg0[0x2F1] = 0;
            r = func_0017E7C0(arg0, 0);
            if (r == 1) {
                arg0[6] = 0x14;
            } else if (r == 2) {
                arg0[6] = 0x1E;
            } else if (r == 0xA) {
                arg0[6] = 0x28;
            }
        } else if (mode == 3) {
            arg0[0x2F1] = 1;
            r = func_0017E7C0(arg0, 1);
            if (r == 1) {
                arg0[6] = 0x14;
            } else if (r == 2) {
                arg0[6] = 0x1E;
            } else if (r == 0xA) {
                arg0[6] = 0x28;
            }
        }
        arg0[7] = 0;
        break;
    case 2:
        if (!(*(int *)(arg0 + 0x200) & 0x8000)) {
            arg0[6] = st + 1;
            func_001FBD50(arg0, 0x12C, 0, 300.0f);
        }
        break;
    case 3:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            arg0[0x25F] = 0;
            func_001C68C0(arg0);
            *(float *)(arg0 + 0xB4) = *(float *)(*(char **)(D_00275B40 + 4) + 0xC4) - 10.5f;
            *(float *)(arg0 + 0x38) = 1.0f + *(float *)(*(char **)(D_00275B40 + 4) + 8);
            func_00178B90(arg0, 1);
            *(float *)(arg0 + 0xB4) += -0.2f;
            func_00175900(arg0, 1);
            func_001749A0(arg0, 0x8C, 0, 0.0f);
            if (arg0[0x23B] == 0x39) {
                *(int *)0x700038A0 = 0;
                *(int *)0x700038A4 = 0;
                *(float *)0x700038A8 = -5.0f;
                *(int *)0x700038AC = 0;
                func_001026A0(D_700038B0, arg0 + 0xD0, D_700038A0);
                func_001028B8(arg0 + 0xB0, arg0 + 0xB0, D_700038B0);
                arg0[5] = 7;
                arg0[6] = 0;
                arg0[0x1F0] = 0xD;
                arg0[0x25F] = 2;
                *(float *)(arg0 + 0x2EC) = 0.0f;
                *(float *)(arg0 + 0x2F4) = *(float *)(arg0 + 0xB4);
                break;
            }
            if (func_001760C0(arg0, arg0 + 0xB0, 1, 18.0f) != 0) {
                arg0[0x236] = 1;
                arg0[0x235] |= 2;
            }
            arg0[6] = arg0[6] + 1;
            *(float *)(arg0 + 0x2EC) = 0.0f;
            break;
        }
        if (*(float *)(arg0 + 0x3C) <= 25.0f) {
            *(float *)(arg0 + 0x204) = 0.5f;
            break;
        }
        if (func_0017F320(arg0) != 0) {
            arg0[6] = 0xA;
            arg0[0x1F0] = 0x13;
            break;
        }
        if (*(unsigned char *)0x70003B8D != 0) {
            *(int *)(arg0 + 0x24C) = 0;
            arg0[0x23F] = 2;
        } else {
            func_00174FD0(arg0);
        }
        if (*(int *)(arg0 + 0x24C) == 0) {
            f = *(float *)(arg0 + 0x26C);
            t = D_00248600[arg0[0x23F]];
            if (f <= t) {
                f = t;
                *(float *)(arg0 + 0x26C) = f;
            } else {
                *(float *)(arg0 + 0x26C) = f;
            }
        }
        *(float *)(arg0 + 0x204) = *(float *)(arg0 + 0x26C);
        break;
    case 4:
        *(float *)(arg0 + 0xB4) += -0.2f;
        if (func_00175900(arg0, 1) != 0) {
            func_00182870(arg0, 1);
            func_00174AC0(arg0, 0);
            mode = arg0[0x23F];
            if (mode >= 2) {
                arg0[6] = arg0[6] + 1;
                func_0017C440(arg0, 1);
            } else {
                arg0[0x25C] = 0;
                func_0017C540(arg0);
            }
        } else {
            func_001796C0(arg0);
        }
        break;
    case 5:
        func_00178B90(arg0, 1);
        if (!(*(int *)(arg0 + 0x200) & 0x8000)) {
            func_0017C540(arg0);
        }
        break;
    case 0xA:
        arg0[6] = st + 1;
        func_001749A0(arg0, 0x80, 0, 4.0f);
        *(float *)(arg0 + 0x21C) = 0.0f;
        *(float *)(arg0 + 0x38) = 0.0f;
        *(float *)(arg0 + 0x2E4) = 0.0f;
        *(float *)(arg0 + 0x2F4) = *(float *)(arg0 + 0xB4);
        break;
    case 0xB:
        flags = *(int *)(arg0 + 0x200);
        if (flags & 0x1000) {
            arg0[5] = 7;
            arg0[6] = 0;
            arg0[0x1F0] = 0xD;
            break;
        }
        if (!(flags & 0x8000)) {
            *(float *)(arg0 + 0x38) = *(float *)(*(char **)D_00275B40 + 8) - *(float *)(arg0 + 0x21C);
            *(float *)(arg0 + 0x21C) = *(float *)(*(char **)D_00275B40 + 8);
            func_00178B90(arg0, 1);
            *(float *)(arg0 + 0x2EC) = *(float *)(*(char **)D_00275B40 + 4) - *(float *)(arg0 + 0x2E4);
            *(float *)(arg0 + 0x2E4) = *(float *)(*(char **)D_00275B40 + 4);
            *(float *)(arg0 + 0xB4) += *(float *)(arg0 + 0x2EC);
        }
        break;
    case 0x14:
        if (func_0017F240(arg0, 0) != 0) {
            break;
        }
        if (func_0017F320(arg0) != 0) {
            arg0[6] = 0xA;
            arg0[0x1F0] = 0x13;
            break;
        }
        sub = arg0[7];
        switch (sub) {
        case 0:
            arg0[7] = sub + 1;
            func_0017DF70(arg0, arg0[0x2F1], 8.0f);
            break;
        case 1:
            if (!(*(int *)(arg0 + 0x200) & 0x8000)) {
                arg0[7] = sub + 1;
            }
            break;
        case 2:
            func_00174FD0(arg0);
            if (*(int *)(arg0 + 0x24C) == D_00275498[arg0[0x2F1]]) {
                r = func_0017E7C0(arg0, arg0[0x2F1]);
                if (r == 1) {
                    if (arg0[0x2F1] == 0) {
                        *(float *)(arg0 + 0x38) = -D_002485E0[arg0[0x23F]];
                        *(float *)(arg0 + 0x204) = D_002485F0[arg0[0x23F]];
                    } else {
                        *(float *)(arg0 + 0x38) = D_002485E0[arg0[0x23F]];
                        *(float *)(arg0 + 0x204) = D_002485F0[arg0[0x23F]];
                    }
                    *(float *)(arg0 + 0xB0) += *(float *)(arg0 + 0x38) * func_0011DE90(*(float *)(arg0 + 0xC4));
                    *(float *)(arg0 + 0xB8) -= *(float *)(arg0 + 0x38) * func_0011E2A8(*(float *)(arg0 + 0xC4));
                    func_00182250(arg0);
                } else if (r == 2) {
                    arg0[6] = 0x1E;
                    arg0[7] = 0;
                } else if (r == 0xA) {
                    arg0[6] = 0x28;
                    arg0[7] = 0;
                } else {
                    arg0[6] = 0;
                    func_001749A0(arg0, func_00188550(arg0), 0, 16.0f);
                    func_00182AF0(arg0);
                    break;
                }
            } else {
                arg0[6] = 0;
                func_001749A0(arg0, func_00188550(arg0), 0, 16.0f);
                func_00182AF0(arg0);
                break;
            }
            if (*(int *)(arg0 + 0x200) & 0x1000) {
                func_00182AF0(arg0);
            }
            break;
        }
        break;
    case 0x1E:
        if (func_0017F240(arg0, 0) == 0) {
            arg0[6] = arg0[6] + 1;
            func_0017DFB0(arg0, arg0[0x2F1], 4.0f);
        }
        break;
    case 0x1F:
        if (func_0017F240(arg0, 0) == 0 && (*(int *)(arg0 + 0x200) & 0x1000)) {
            arg0[6] = arg0[6] + 1;
            func_0017E0D0(arg0, arg0[0x2F1], 1.0f);
        }
        break;
    case 0x20:
        if (func_0017F240(arg0, 0) == 0) {
            if (D_00810E74[0] & *(unsigned short *)0x70003B76) {
                arg0[6] = arg0[6] + 1;
                func_0017E1D0(arg0, arg0[0x2F1], 1.0f);
                arg0[0x1F0] = 0x12;
                break;
            }
            if (func_0017F320(arg0) != 0) {
                arg0[6] = 0xA;
                arg0[0x1F0] = 0x13;
                break;
            }
            func_00174FD0(arg0);
            if (*(int *)(arg0 + 0x24C) != D_00275498[arg0[0x2F1]]) {
                arg0[6] = 0x27;
                func_0017E150(arg0, arg0[0x2F1], 1.0f);
            }
        }
        break;
    case 0x21:
        if ((arg0[0x315] == 0 && *(float *)(arg0 + 0x3C) <= 6.0f) ||
            (arg0[0x315] != 0 && (*(int *)(arg0 + 0x200) & 0x1000))) {
            arg0[6] = arg0[6] + 1;
            func_001FBD50(arg0, 0x187, 0, 300.0f);
            *(float *)(arg0 + 0x2F4) = *(float *)(arg0 + 0x2E0);
            *(float *)(arg0 + 0x2F8) = *(float *)(arg0 + 0x2E8);
            *(float *)(arg0 + 0x258) = *(float *)(arg0 + 0x2E4);
            *(short *)(arg0 + 0x28) = 8;
            *(float *)(arg0 + 0x2E0) = (*(float *)(arg0 + 0x2F4) - *(float *)(arg0 + 0xB0)) / 8.0f;
            *(float *)(arg0 + 0x2E8) = (*(float *)(arg0 + 0x2F8) - *(float *)(arg0 + 0xB8)) / 8.0f;
            *(float *)(arg0 + 0x2E4) = (*(float *)(arg0 + 0x258) - *(float *)(arg0 + 0xB4)) / 8.0f;
            t = func_001B1470(*(float *)(arg0 + 0x218) - *(float *)(arg0 + 0xC4));
            *(float *)0x70003A20 = t;
            if (!(t < 0.0f)) {
                *(float *)(arg0 + 0x26C) = t / 8.0f;
            } else {
                *(float *)(arg0 + 0x26C) = -t / 8.0f;
            }
            sub = arg0[0x1F1];
            if (sub == 1) {
                func_001749A0(arg0, 0xE5, 0, 8.0f);
            } else if (sub == 6) {
                func_001749A0(arg0, 0x96, 0, 8.0f);
            } else if (sub == 2) {
                if (arg0[0x315] == 0) {
                    func_0017E150(arg0, arg0[0x2F1], 8.0f);
                } else {
                    func_0017E150(arg0, 1 - arg0[0x2F1], 8.0f);
                }
            } else if (sub == 5) {
                arg0[0x315] = 0;
                func_0017E150(arg0, arg0[0x2F1], 8.0f);
            } else {
                func_0017E150(arg0, arg0[0x2F1], 8.0f);
            }
        }
        break;
    case 0x22:
        if (*(short *)(arg0 + 0x28) == 0) {
            arg0[6] = st + 1;
            *(float *)(arg0 + 0xB0) = *(float *)(arg0 + 0x2F4);
            *(float *)(arg0 + 0xB8) = *(float *)(arg0 + 0x2F8);
            *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0x258);
            *(float *)(arg0 + 0xC4) = *(float *)(arg0 + 0x218);
            sub = arg0[0x1F1];
            if (sub == 1) {
                func_00182A70(arg0);
            } else if (sub == 6) {
                func_001FBD50(arg0, 0x119, 0, 300.0f);
            } else if (sub == 2) {
                func_001FBD50(arg0, 0xFF, 0, 300.0f);
            } else if (sub == 5) {
                func_001FBD50(arg0, 0xFF, 0, 300.0f);
            } else {
                func_001FBD50(arg0, 0xFF, 0, 300.0f);
            }
            break;
        }
        *(float *)(arg0 + 0xB0) += *(float *)(arg0 + 0x2E0);
        *(float *)(arg0 + 0xB8) += *(float *)(arg0 + 0x2E8);
        *(float *)(arg0 + 0xB4) += *(float *)(arg0 + 0x2E4);
        *(float *)(arg0 + 0xC4) = func_001B12B0(*(float *)(arg0 + 0x218), *(float *)(arg0 + 0xC4),
                                                *(float *)(arg0 + 0x26C));
        *(short *)(arg0 + 0x28) = *(short *)(arg0 + 0x28) - 1;
        break;
    case 0x23:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            sub = arg0[0x1F1];
            if (sub == 1) {
                func_00182A70(arg0);
                arg0[5] = 0xC;
                arg0[6] = 0;
                arg0[0x1F0] = 0x17;
                arg0[0x2F1] = 0;
                func_0017FC80(arg0, 16.0f);
            } else if (sub == 6) {
                func_001FBD50(arg0, 0x119, 0, 300.0f);
                arg0[5] = 0xE;
                arg0[6] = 0;
                arg0[0x1F0] = 0x1D;
                arg0[0xD] = 2;
                arg0[0x2F1] = 0;
                func_0017FF80(arg0, 16.0f);
            } else if (sub == 2) {
                arg0[5] = 9;
                arg0[6] = 0;
                arg0[0x1F0] = 0x10;
                arg0[0xD] = 0;
                func_001749A0(arg0, func_00188550(arg0), 0, 16.0f);
            } else if (sub == 5) {
                arg0[5] = 0x18;
                arg0[6] = 0;
                arg0[0x1F0] = 0x2C;
                arg0[0x1F1] = 0;
                arg0[0xD] = 2;
                func_001749A0(arg0, func_00188550(arg0), 0, 16.0f);
            } else {
                arg0[5] = 9;
                arg0[6] = 0;
                arg0[0x1F0] = 0x10;
                arg0[0xD] = 0;
                func_001749A0(arg0, func_00188550(arg0), 0, 16.0f);
            }
        }
        break;
    case 0x27:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            arg0[6] = 0;
            func_001749A0(arg0, func_00188550(arg0), 0, 16.0f);
        }
        break;
    case 0x28:
        if (func_0017F240(arg0, 0) == 0) {
            arg0[6] = arg0[6] + 1;
            func_0017DFB0(arg0, arg0[0x2F1], 4.0f);
        }
        break;
    case 0x29:
        if (func_0017F240(arg0, 0) == 0 && (*(int *)(arg0 + 0x200) & 0x1000)) {
            arg0[6] = arg0[6] + 1;
            func_0017E0D0(arg0, arg0[0x2F1], 1.0f);
        }
        break;
    case 0x2A:
        if (func_0017F240(arg0, 0) == 0) {
            if (D_00810E74[0] & *(unsigned short *)0x70003B76) {
                if (arg0[0x2F1] == 0) {
                    *(float *)0x700038A0 = -9.0f;
                    *(float *)0x700038B0 = -9.0f;
                    *(float *)0x700038A4 = 20.0f;
                    *(float *)0x700038B4 = 20.0f;
                    *(float *)0x700038A8 = -2.0f;
                    *(float *)0x700038AC = 1.0f;
                    *(float *)0x700038B8 = 3.5f;
                    *(float *)0x700038BC = 1.0f;
                } else {
                    *(float *)0x700038A0 = 9.0f;
                    *(float *)0x700038B0 = 9.0f;
                    *(float *)0x700038A4 = 20.0f;
                    *(float *)0x700038B4 = 20.0f;
                    *(float *)0x700038A8 = -2.0f;
                    *(float *)0x700038AC = 1.0f;
                    *(float *)0x700038B8 = 3.5f;
                    *(float *)0x700038BC = 1.0f;
                }
                func_001026A0(D_700038C0, arg0 + 0xD0, D_700038A0);
                func_001026A0(D_700038D0, arg0 + 0xD0, D_700038B0);
                if ((func_0019AFE0(arg0, D_700038C0, D_700038D0, 7) & 6) &&
                    func_00178910(arg0, 1) != 0) {
                    arg0[0x1F0] = 0x12;
                    arg0[0x1F1] = 2;
                    func_0017E1D0(arg0, arg0[0x2F1], 1.0f);
                    arg0[6] = 0x21;
                }
                break;
            }
            if (func_0017F320(arg0) != 0) {
                arg0[6] = 0xA;
                arg0[0x1F0] = 0x13;
                break;
            }
            func_00174FD0(arg0);
            if (*(int *)(arg0 + 0x24C) != D_00275498[arg0[0x2F1]]) {
                arg0[6] = 0x31;
                func_0017E150(arg0, arg0[0x2F1], 1.0f);
            }
        }
        break;
    case 0x31:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            arg0[6] = 0;
            func_001749A0(arg0, func_00188550(arg0), 0, 16.0f);
        }
        break;
    }
}
