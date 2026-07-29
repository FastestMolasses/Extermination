// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

extern int func_00181180(void *p, int a);
extern void func_00174FD0(void *p);
extern int func_001806E0(void *p);
extern int func_00180790(void *p);
extern int func_00180850(void *p, int flag);
extern int func_001809B0(void *p, int flag);
extern void func_001031E0(void *a, void *b);
extern void func_00102948(void *a, void *b);
extern void func_00180000(void *p, float f);
extern void func_00180040(void *p, float f);
extern void func_00180080(void *p, float f);
extern void func_001800C0(void *p, float f);
extern void func_00180100(void *p, int sel, float f);
extern void func_00180180(void *p, int sel, float f);
extern void func_00180200(void *p, int sel, float f);
extern void func_00180280(void *p, int sel, float f);
extern void func_0017FF80(void *p, float f);
extern void func_00182AB0(void *p);
extern void func_00182430();
extern int func_001749A0(void *p, short clip, int flags, float blend);
extern int func_001FBD50(void *p, int id, int flags, float vol);
extern unsigned char func_00175900(void *p, int a);
extern void func_00187EE0(void *p, void *a, void *b);
extern void func_00174AB0(void *p);
extern void func_00174A50(void *p, float f);
extern void func_001C68C0(void *p);
extern short func_00188550(void *p);
extern float func_0011DE90(float a);
extern float func_0011E2A8(float a);
extern float func_001B1470(float a);
extern float func_001B12B0(float a, float b, float c);
extern char *D_00275B40;
extern unsigned short D_00810E74[8];
extern float D_00248610[];
extern float D_00248620[];

void func_00168050(unsigned char *arg0) {
    unsigned char st;
    unsigned char sub;
    int r;
    int mode;
    unsigned char lvl;
    float f;
    float t;

    st = arg0[6];
    switch (st) {
    case 0:
        arg0[6] = st + 1;
        func_001031E0(arg0 + 0x290, arg0 + 0xB0);
        /* fallthrough */
    case 1:
        if (func_00181180(arg0, 0) != 0) {
            break;
        }
        func_00174FD0(arg0);
        mode = *(int *)(arg0 + 0x24C);
        if (mode == 0) {
            r = func_001806E0(arg0);
            if (r == 0) {
                arg0[6] = 0xA;
            } else if (r == 2) {
                arg0[6] = 0x1E;
            }
        } else if (mode == 1) {
            r = func_00180790(arg0);
            if (r == 0) {
                arg0[6] = 0x14;
            } else if (r == 2) {
                arg0[6] = 0x28;
            }
        } else if (mode == 2) {
            if (func_00180850(arg0, 0) == 0) {
                arg0[6] = 0x32;
            } else {
                arg0[6] = 0x46;
            }
        } else if (mode == 3) {
            if (func_00180850(arg0, 1) == 0) {
                arg0[6] = 0x3C;
            } else {
                arg0[6] = 0x50;
            }
        }
        arg0[7] = 0;
        *(float *)(arg0 + 0x21C) = 0.0f;
        *(float *)(arg0 + 0x38) = 0.0f;
        *(float *)(arg0 + 0x2E4) = 0.0f;
        break;
    case 0xA:
        if (func_00181180(arg0, 1) != 0) {
            break;
        }
        sub = arg0[7];
        switch (sub) {
        case 0:
            arg0[7] = sub + 1;
            func_00180000(arg0, 4.0f);
            break;
        case 1:
            if (!(*(int *)(arg0 + 0x200) & 0x8000)) {
                arg0[7] = sub + 1;
            }
            break;
        case 2:
            func_00174FD0(arg0);
            if (*(int *)(arg0 + 0x200) & 0x1000) {
                func_00182AB0(arg0);
                arg0[0x2F1] = 1 - arg0[0x2F1];
                if (*(int *)(arg0 + 0x24C) == 0) {
                    r = func_001806E0(arg0);
                    if (r == 0) {
                        func_00180000(arg0, 1.0f);
                        *(float *)(arg0 + 0x21C) = 0.0f;
                    } else if (r == 2) {
                        arg0[6] = 0x1E;
                        arg0[7] = 0;
                        arg0[0x1F0] = 0x18;
                    } else {
                        arg0[6] = 0;
                        func_0017FF80(arg0, 8.0f);
                    }
                } else {
                    arg0[6] = 0;
                    func_0017FF80(arg0, 8.0f);
                }
                func_001031E0(arg0 + 0x290, arg0 + 0xB0);
                break;
            }
            *(float *)(arg0 + 0x2E4) = *(float *)(*(char **)D_00275B40 + 4) - *(float *)(arg0 + 0x21C);
            *(float *)(arg0 + 0x21C) = *(float *)(*(char **)D_00275B40 + 4);
            *(float *)(arg0 + 0xB4) += *(float *)(arg0 + 0x2E4);
            if (*(int *)(arg0 + 0x24C) == 0) {
                lvl = arg0[0x23F];
                if (lvl == 3) {
                    *(float *)(arg0 + 0x204) = 2.0f;
                } else if (lvl == 2) {
                    *(float *)(arg0 + 0x204) = 1.5f;
                } else {
                    *(float *)(arg0 + 0x204) = 1.0f;
                }
            }
            break;
        }
        break;
    case 0x14:
        if (func_00181180(arg0, 0) != 0) {
            break;
        }
        sub = arg0[7];
        switch (sub) {
        case 0:
            arg0[7] = sub + 1;
            func_00180040(arg0, 4.0f);
            break;
        case 1:
            if (!(*(int *)(arg0 + 0x200) & 0x8000)) {
                arg0[7] = sub + 1;
            }
            break;
        case 2:
            func_00174FD0(arg0);
            if (*(int *)(arg0 + 0x200) & 0x1000) {
                func_00182AB0(arg0);
                arg0[0x2F1] = 1 - arg0[0x2F1];
                if (*(int *)(arg0 + 0x24C) == 1) {
                    r = func_00180790(arg0);
                    if (r == 0) {
                        func_00180040(arg0, 1.0f);
                        *(float *)(arg0 + 0x21C) = 0.0f;
                    } else if (r == 1) {
                        arg0[6] = 0;
                        func_0017FF80(arg0, 8.0f);
                    } else {
                        arg0[6] = 0x28;
                        arg0[7] = 0;
                        arg0[0x1F0] = 0x18;
                    }
                } else {
                    arg0[6] = 0;
                    func_0017FF80(arg0, 8.0f);
                }
                func_001031E0(arg0 + 0x290, arg0 + 0xB0);
                break;
            }
            *(float *)(arg0 + 0x2E4) = *(float *)(*(char **)D_00275B40 + 4) - *(float *)(arg0 + 0x21C);
            *(float *)(arg0 + 0x21C) = *(float *)(*(char **)D_00275B40 + 4);
            *(float *)(arg0 + 0xB4) += *(float *)(arg0 + 0x2E4);
            if (*(int *)(arg0 + 0x24C) == 1) {
                lvl = arg0[0x23F];
                if (lvl == 3) {
                    *(float *)(arg0 + 0x204) = 2.0f;
                } else if (lvl == 2) {
                    *(float *)(arg0 + 0x204) = 1.5f;
                } else {
                    *(float *)(arg0 + 0x204) = 1.0f;
                }
            }
            break;
        }
        break;
    case 0x1E:
        arg0[6] = st + 1;
        func_001749A0(arg0, 0xA5, 0, 4.0f);
        break;
    case 0x1F:
        if (!(*(int *)(arg0 + 0x200) & 0x8000)) {
            arg0[6] = st + 1;
        }
        break;
    case 0x20:
        sub = arg0[7];
        switch (sub) {
        case 0:
            if (*(float *)(arg0 + 0x3C) <= 78.0f) {
                arg0[7] = sub + 1;
                func_00182AB0(arg0);
            }
            break;
        case 1:
            if (*(float *)(arg0 + 0x3C) <= 64.0f) {
                arg0[7] = sub + 1;
                func_00182AB0(arg0);
            }
            break;
        case 2:
            if (*(float *)(arg0 + 0x3C) <= 42.0f) {
                arg0[7] = sub + 1;
                func_00182430(arg0, 2);
            }
            break;
        case 3:
            if (*(float *)(arg0 + 0x3C) <= 25.0f) {
                arg0[7] = sub + 1;
                func_00182430(arg0, 2);
            }
            break;
        case 4:
            if (*(float *)(arg0 + 0x3C) <= 14.0f) {
                arg0[6] = st + 1;
                func_00182430(arg0, 2);
            }
            break;
        }
        break;
    case 0x21:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            func_001C68C0(arg0);
            func_00102948(arg0 + 0xB0, *(char **)(D_00275B40 + 4) + 0xC0);
            *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0xB4) - 10.5f;
            *(float *)(arg0 + 0xB4) += -0.2f;
            func_00175900(arg0, 1);
            func_00182430(arg0, 2);
            func_00187EE0(arg0, arg0 + 0xB0, arg0 + 0xD0);
            func_00174AB0(arg0);
            func_00174A50(arg0, 18.0f);
            arg0[5] = 0;
            arg0[6] = 0;
            arg0[0x1F0] = 0;
        }
        break;
    case 0x28:
        arg0[6] = st + 1;
        func_001749A0(arg0, 0xA6, 0, 4.0f);
        break;
    case 0x29:
        if (!(*(int *)(arg0 + 0x200) & 0x8000)) {
            arg0[6] = st + 1;
        }
        break;
    case 0x2A:
        if (*(float *)(arg0 + 0x3C) <= 22.0f) {
            arg0[6] = st + 1;
            func_00102948(arg0 + 0x290, arg0 + 0xB0);
            func_001C68C0(arg0);
            func_00102948(arg0 + 0xB0, *(char **)(D_00275B40 + 4) + 0xC0);
            *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0xB4) - 11.5f;
            *(float *)(arg0 + 0xB4) += -0.2f;
            func_00175900(arg0, 1);
            func_00182430(arg0, 2);
            func_00187EE0(arg0, arg0 + 0xB0, arg0 + 0xD0);
            func_00102948(arg0 + 0xB0, arg0 + 0x290);
        }
        break;
    case 0x2B:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            func_001C68C0(arg0);
            func_00102948(arg0 + 0xB0, *(char **)(D_00275B40 + 4) + 0xC0);
            *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0xB4) - 11.5f;
            *(float *)(arg0 + 0xB4) += -0.2f;
            func_00175900(arg0, 1);
            func_00174AB0(arg0);
            func_00174A50(arg0, 18.0f);
            arg0[5] = 0;
            arg0[6] = 0;
            arg0[0x1F0] = 0;
        }
        break;
    case 0x32:
        if (func_00181180(arg0, 0) != 0) {
            break;
        }
        sub = arg0[7];
        switch (sub) {
        case 0:
            arg0[7] = sub + 1;
            func_00180080(arg0, 8.0f);
            break;
        case 1:
            if (!(*(int *)(arg0 + 0x200) & 0x8000)) {
                arg0[7] = sub + 1;
            }
            break;
        case 2:
            func_00174FD0(arg0);
            if (*(int *)(arg0 + 0x24C) == 2) {
                if (func_00180850(arg0, 0) == 0) {
                    *(float *)(arg0 + 0x38) = -D_00248610[arg0[0x23F]];
                    *(float *)(arg0 + 0x204) = D_00248620[arg0[0x23F]];
                    *(float *)(arg0 + 0xB0) += *(float *)(arg0 + 0x38) * func_0011DE90(*(float *)(arg0 + 0xC4));
                    *(float *)(arg0 + 0xB8) -= *(float *)(arg0 + 0x38) * func_0011E2A8(*(float *)(arg0 + 0xC4));
                } else {
                    arg0[6] = 0x46;
                    arg0[7] = 0;
                }
            } else {
                arg0[6] = 0;
                func_0017FF80(arg0, 8.0f);
                func_00182AB0(arg0);
                break;
            }
            if (*(int *)(arg0 + 0x200) & 0x1000) {
                func_00182AB0(arg0);
            }
            break;
        }
        break;
    case 0x3C:
        if (func_00181180(arg0, 0) != 0) {
            break;
        }
        sub = arg0[7];
        switch (sub) {
        case 0:
            arg0[7] = sub + 1;
            func_001800C0(arg0, 8.0f);
            break;
        case 1:
            if (!(*(int *)(arg0 + 0x200) & 0x8000)) {
                arg0[7] = sub + 1;
            }
            break;
        case 2:
            func_00174FD0(arg0);
            if (*(int *)(arg0 + 0x24C) == 3) {
                if (func_00180850(arg0, 1) == 0) {
                    *(float *)(arg0 + 0x38) = D_00248610[arg0[0x23F]];
                    *(float *)(arg0 + 0x204) = D_00248620[arg0[0x23F]];
                    *(float *)(arg0 + 0xB0) += *(float *)(arg0 + 0x38) * func_0011DE90(*(float *)(arg0 + 0xC4));
                    *(float *)(arg0 + 0xB8) -= *(float *)(arg0 + 0x38) * func_0011E2A8(*(float *)(arg0 + 0xC4));
                } else {
                    arg0[6] = 0x50;
                    arg0[7] = 0;
                }
            } else {
                arg0[6] = 0;
                func_0017FF80(arg0, 8.0f);
                func_00182AB0(arg0);
                break;
            }
            if (*(int *)(arg0 + 0x200) & 0x1000) {
                func_00182AB0(arg0);
            }
            break;
        }
        break;
    case 0x46:
        if (func_00181180(arg0, 0) != 0) {
            break;
        }
        sub = arg0[7];
        switch (sub) {
        case 0:
            arg0[7] = sub + 1;
            func_00180100(arg0, 0, 4.0f);
            break;
        case 1:
            if (!(*(int *)(arg0 + 0x200) & 0x8000)) {
                arg0[7] = sub + 1;
                func_001FBD50(arg0, 0x120, 0, 300.0f);
            }
            break;
        case 2:
            if (*(int *)(arg0 + 0x200) & 0x1000) {
                arg0[7] = sub + 1;
                func_00180180(arg0, 0, 1.0f);
            }
            break;
        case 3:
            func_00174FD0(arg0);
            if (D_00810E74[0] & *(unsigned short *)0x70003B76) {
                if (func_001809B0(arg0, 0) != 0) {
                    arg0[6] = 0x5A;
                    arg0[7] = 0;
                    arg0[0x1F0] = 0x1F;
                    func_00180280(arg0, 0, 1.0f);
                }
            } else if (*(int *)(arg0 + 0x24C) != 2) {
                arg0[7] = arg0[7] + 1;
                func_00180200(arg0, 0, 1.0f);
            }
            break;
        case 4:
            if (*(int *)(arg0 + 0x200) & 0x1000) {
                arg0[6] = 0;
                func_0017FF80(arg0, 16.0f);
            }
            break;
        }
        break;
    case 0x50:
        if (func_00181180(arg0, 0) != 0) {
            break;
        }
        sub = arg0[7];
        switch (sub) {
        case 0:
            arg0[7] = sub + 1;
            func_00180100(arg0, 1, 4.0f);
            break;
        case 1:
            if (!(*(int *)(arg0 + 0x200) & 0x8000)) {
                arg0[7] = sub + 1;
                func_001FBD50(arg0, 0x121, 0, 300.0f);
            }
            break;
        case 2:
            if (*(int *)(arg0 + 0x200) & 0x1000) {
                arg0[7] = sub + 1;
                func_00180180(arg0, 1, 1.0f);
            }
            break;
        case 3:
            func_00174FD0(arg0);
            if (D_00810E74[0] & *(unsigned short *)0x70003B76) {
                if (func_001809B0(arg0, 1) != 0) {
                    arg0[6] = 0x5A;
                    arg0[7] = 0;
                    arg0[0x1F0] = 0x1F;
                    func_00180280(arg0, 1, 1.0f);
                }
            } else if (*(int *)(arg0 + 0x24C) != 3) {
                arg0[7] = arg0[7] + 1;
                func_00180200(arg0, 1, 1.0f);
            }
            break;
        case 4:
            if (*(int *)(arg0 + 0x200) & 0x1000) {
                arg0[6] = 0;
                func_0017FF80(arg0, 16.0f);
            }
            break;
        }
        break;
    case 0x5A:
        if (*(float *)(arg0 + 0x3C) <= 6.0f) {
            arg0[6] = st + 1;
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
            func_001749A0(arg0, 0x7A, 0, 8.0f);
        }
        break;
    case 0x5B:
        if (*(short *)(arg0 + 0x28) == 0) {
            arg0[6] = st + 1;
            *(float *)(arg0 + 0xB0) = *(float *)(arg0 + 0x2F4);
            *(float *)(arg0 + 0xB8) = *(float *)(arg0 + 0x2F8);
            *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0x258);
            *(float *)(arg0 + 0xC4) = *(float *)(arg0 + 0x218);
            func_001FBD50(arg0, 0xFE, 0, 300.0f);
            break;
        }
        *(float *)(arg0 + 0xB0) += *(float *)(arg0 + 0x2E0);
        *(float *)(arg0 + 0xB8) += *(float *)(arg0 + 0x2E8);
        *(float *)(arg0 + 0xB4) += *(float *)(arg0 + 0x2E4);
        *(float *)(arg0 + 0xC4) = func_001B12B0(*(float *)(arg0 + 0x218), *(float *)(arg0 + 0xC4), *(float *)(arg0 + 0x26C));
        *(short *)(arg0 + 0x28) = *(short *)(arg0 + 0x28) - 1;
        break;
    case 0x5C:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            arg0[5] = 9;
            arg0[6] = 0;
            arg0[0x1F0] = 0x10;
            arg0[0xD] = 0;
            func_001749A0(arg0, func_00188550(arg0), 0, 16.0f);
        }
        break;
    }
}
