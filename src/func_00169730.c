// NEARMISS func_00169730  (vram 0x00169730, 0xD78 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.99% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-coloring permutation (2 of 862 instructions). In case 1 the target loads the pad word D_00810E74 into $a1 ('lhu a1, %lo(D_00810E74)(at)' + 'and v1,a1,v1'); mwcc 2.3.3 picks the equally-dead $a0. Every other instruction is byte-identical. Identical wall (same case-1 pad test) to the alrea...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

//
// NEARMISS residual (mwcc 2.3.3, objdiff 99.99%): a single register-coloring
// choice in case 1 — the target loads the pad word D_00810E74 into $a1
// ('lhu a1' + 'and v1,a1,v1'), mwcc 2.3.3 picks the equally-dead $a0. All
// 860 other instructions are byte-identical. Same wall (and same case-1 pad
// test) as the sibling state machine func_0016AE40; operand-order, hoisted
// -local, explicit-!=0 and scalar-vs-array shapes all reproduce $a0.
//
// Boss/large-enemy behaviour state machine (sibling of func_0016AE40),
// dispatched on the state byte at e+6 (cases 0,1,10,11,20,30,31,40,50,60,
// 70,80,90,91,92,93), with nested phase machines on e+7 inside cases
// 20/40/50/60/70.
extern int func_001885B0(unsigned char *e);
extern void func_001749A0(unsigned char *e, int id, int z, float f);
extern void func_001031E0(void *dst, void *src);
extern int func_00181D70(unsigned char *e);
extern int func_001696A0(unsigned char *e);
extern int func_001751A0();
extern int func_001814E0(unsigned char *e, int a);
extern int func_001818D0(unsigned char *e);
extern int func_00181730(unsigned char *e, int a);
extern void func_00179880(unsigned char *e, void *p);
extern int func_00175900(unsigned char *e, int f);
extern void func_00181B80(unsigned char *e);
extern void func_00181BA0(unsigned char *e);
extern void func_001811F0(unsigned char *e);
extern void func_00181430(unsigned char *e);
extern void func_00178B90(unsigned char *e, int a);
extern int func_00179B90();
extern void func_001FBD50(unsigned char *e, int a, int b, float f);
extern float func_001B1470(float a);
extern float func_001B12B0(float goal, float cur, float rate);
extern float func_0011DE90(float a);
extern float func_0011E2A8(float a);
extern int func_001787B0(unsigned char *e, int a);
extern void func_0016A4B0(unsigned char *e);
extern void func_00182A70(unsigned char *e);
extern void func_0017FC80(unsigned char *e, float f);
extern int float_to_int(float f);
extern void build_trs_matrix(void *m, void *pos, void *rot, void *scale);
extern int D_00275B40;
extern unsigned short D_00810E74[];

void func_00169730(unsigned char *e)
{
    unsigned char st;

    st = e[6];
    switch (st) {
    case 0:
        e[6] = st + 1;
        e[7] = 0;
        e[0x1F0] = 0x21;
        e[0x1F1] = 0;
        e[0x25C] = 0;
        e[0x2F1] = 0;
        func_001749A0(e, func_001885B0(e), 0, 16.0f);
        func_001031E0(e + 0x290, e + 0xB0);
        break;
    case 1:
        if (func_00181D70(e) != 0) {
            break;
        }
        if (func_001696A0(e) != 0) {
            break;
        }
        if (D_00810E74[0] & *(unsigned short *)0x70003B76) {
            e[6] = 0xA;
            e[0x1F0] = 0x23;
            break;
        }
        func_001751A0();
        {
            int n = *(int *)(e + 0x24C);
            if (n == 0) {
                if (func_001814E0(e, 0) != 0) {
                    e[6] = 0x14;
                    e[7] = 0;
                    e[0x1F1] = 1;
                    break;
                }
                if (func_001818D0(e) != 0) {
                    break;
                }
                e[6] = 0x50;
                e[7] = 0;
                e[0x1F0] = 0x28;
            } else if (n == 1) {
                e[6] = 0x1E;
                e[0x1F0] = 0x24;
            } else if (n == 2) {
                *(float *)(e + 0x38) = -0.08f;
                if (func_00181730(e, 0) != 0) {
                    e[6] = 0x28;
                    e[7] = 0;
                } else {
                    e[6] = 0x3C;
                    e[7] = 0;
                }
            } else if (n == 3) {
                *(float *)(e + 0x38) = 0.08f;
                if (func_00181730(e, 1) != 0) {
                    e[6] = 0x32;
                    e[7] = 0;
                } else {
                    e[6] = 0x46;
                    e[7] = 0;
                }
            }
        }
        break;
    case 10:
        e[6] = st + 1;
        func_001749A0(e, 0xD6, 0, 1.0f);
        *(float *)(e + 0x2F4) = *(float *)(e + 0xB4);
        *(float *)(e + 0x2EC) = 0.0f;
        break;
    case 11:
        if (*(int *)(e + 0x200) & 0x1000) {
            e[5] = 7;
            e[6] = 0;
            e[0x1F0] = 0xD;
        }
        func_00179880(e, e + 0x2EC);
        func_00175900(e, 1);
        break;
    case 20:
        if (func_00181D70(e) != 0) {
            break;
        }
        {
            unsigned char ph = e[7];
            switch (ph) {
            case 0:
                e[7] = ph + 1;
                e[0x2F1] = 1;
                func_001749A0(e, 0xBC, 0, 8.0f);
                e[0x25C] = 2;
                *(float *)(e + 0x26C) = 1.0f;
                *(float *)(e + 0x38) = 0.0f;
                *(float *)(e + 0x21C) = 0.0f;
                func_00181B80(e);
                break;
            case 1:
                if (!(*(int *)(e + 0x200) & 0x8000)) {
                    e[7] = ph + 1;
                }
                break;
            case 2:
                if (*(int *)(e + 0x200) & 0x1000) {
                    func_001FBD50(e, func_00179B90() + 0x124, 0, 300.0f);
                    func_00181BA0(e);
                    func_00181B80(e);
                    build_trs_matrix(e + 0xD0, e + 0xB0, e + 0xC0, e + 0x60);
                    if (func_001696A0(e) != 0) {
                        return;
                    }
                    func_001751A0();
                    if (*(int *)(e + 0x24C) == 0) {
                        int r = func_001814E0(e, 1);
                        if (r == 1) {
                            func_001811F0(e);
                            e[0x25C] = e[0x23F];
                            *(float *)(e + 0x38) = 0.0f;
                            *(float *)(e + 0x21C) = 0.0f;
                            return;
                        }
                        if (r == 2) {
                            if (e[0x23F] == 3) {
                                e[0x23F] = 2;
                            }
                            func_001811F0(e);
                            e[0x25C] = e[0x23F];
                            *(float *)(e + 0x38) = 0.0f;
                            *(float *)(e + 0x21C) = 0.0f;
                            return;
                        }
                        e[7] = e[7] + 1;
                    } else {
                        e[7] = e[7] + 1;
                    }
                } else {
                    *(float *)(e + 0x204) = *(float *)(e + 0x26C);
                    *(float *)(e + 0x38) = *(float *)(*(int *)D_00275B40 + 8) - *(float *)(e + 0x21C);
                    *(float *)(e + 0x21C) = *(float *)(*(int *)D_00275B40 + 8);
                    func_00178B90(e, 0);
                }
                break;
            case 3:
                e[7] = ph + 1;
                func_00181430(e);
                *(float *)(e + 0x38) = 0.0f;
                *(float *)(e + 0x21C) = 0.0f;
                break;
            case 4:
                if (*(int *)(e + 0x200) & 0x1000) {
                    func_001FBD50(e, func_00179B90() + 0x124, 0, 300.0f);
                    func_00181BA0(e);
                    e[6] = 0;
                } else {
                    *(float *)(e + 0x38) = *(float *)(*(int *)D_00275B40 + 8) - *(float *)(e + 0x21C);
                    *(float *)(e + 0x21C) = *(float *)(*(int *)D_00275B40 + 8);
                    func_00178B90(e, 0);
                }
                break;
            }
        }
        break;
    case 30:
        e[6] = st + 1;
        func_001749A0(e, 0xD7, 0, 1.0f);
        break;
    case 31:
        if (*(int *)(e + 0x200) & 0x1000) {
            *(float *)(e + 0xC4) = func_001B1470(3.1415927f + *(float *)(e + 0xC4));
            func_001749A0(e, func_001885B0(e), 0, 0.0f);
            e[6] = 0;
        }
        break;
    case 40:
        if (func_00181D70(e) != 0) {
            break;
        }
        {
            unsigned char ph = e[7];
            switch (ph) {
            case 0:
                e[7] = ph + 1;
                func_001749A0(e, 0xCA, 0, 1.0f);
                break;
            case 1:
                func_001751A0(e);
                if (*(int *)(e + 0x24C) == 2) {
                    if (func_00181730(e, 0) != 0) {
                        *(float *)(e + 0xB0) = *(float *)(e + 0xB0) + *(float *)(e + 0x38) * func_0011DE90(*(float *)(e + 0xC4));
                        *(float *)(e + 0xB8) = *(float *)(e + 0xB8) - *(float *)(e + 0x38) * func_0011E2A8(*(float *)(e + 0xC4));
                    } else {
                        e[6] = 0x3C;
                        e[7] = 0;
                    }
                } else {
                    e[7] = e[7] + 1;
                    func_001749A0(e, func_001885B0(e), 0, 8.0f);
                }
                break;
            case 2:
                if (!(*(int *)(e + 0x200) & 0x8000)) {
                    e[6] = 0;
                }
                break;
            }
        }
        break;
    case 50:
        if (func_00181D70(e) != 0) {
            break;
        }
        {
            unsigned char ph = e[7];
            switch (ph) {
            case 0:
                e[7] = ph + 1;
                func_001749A0(e, 0xCB, 0, 1.0f);
                break;
            case 1:
                func_001751A0(e);
                if (*(int *)(e + 0x24C) == 3) {
                    if (func_00181730(e, 1) != 0) {
                        *(float *)(e + 0xB0) = *(float *)(e + 0xB0) + *(float *)(e + 0x38) * func_0011DE90(*(float *)(e + 0xC4));
                        *(float *)(e + 0xB8) = *(float *)(e + 0xB8) - *(float *)(e + 0x38) * func_0011E2A8(*(float *)(e + 0xC4));
                    } else {
                        e[6] = 0x46;
                        e[7] = 0;
                    }
                } else {
                    e[7] = e[7] + 1;
                    func_001749A0(e, func_001885B0(e), 0, 8.0f);
                }
                break;
            case 2:
                if (!(*(int *)(e + 0x200) & 0x8000)) {
                    e[6] = 0;
                }
                break;
            }
        }
        break;
    case 60:
        if (func_00181D70(e) != 0) {
            break;
        }
        {
            unsigned char ph = e[7];
            switch (ph) {
            case 0:
                e[7] = ph + 1;
                func_001749A0(e, 0xCC, 0, 4.0f);
                break;
            case 1:
                if (*(int *)(e + 0x200) & 0x1000) {
                    func_001749A0(e, 0xCE, 0, 1.0f);
                    e[7] = e[7] + 1;
                }
                break;
            case 2:
                if (D_00810E74[0] & *(unsigned short *)0x70003B76) {
                    if (func_001787B0(e, 0) != 0) {
                        e[6] = 0x5A;
                        e[0x1F0] = 0x12;
                        func_001749A0(e, 0xD0, 0, 1.0f);
                    }
                } else {
                    func_001751A0(e);
                    if (*(int *)(e + 0x24C) != 2) {
                        func_001749A0(e, 0xD2, 0, 1.0f);
                        e[7] = e[7] + 1;
                    }
                }
                break;
            case 3:
                if (*(int *)(e + 0x200) & 0x1000) {
                    e[6] = 0;
                }
                break;
            }
        }
        break;
    case 70:
        if (func_00181D70(e) != 0) {
            break;
        }
        {
            unsigned char ph = e[7];
            switch (ph) {
            case 0:
                e[7] = ph + 1;
                func_001749A0(e, 0xCD, 0, 4.0f);
                break;
            case 1:
                if (*(int *)(e + 0x200) & 0x1000) {
                    func_001749A0(e, 0xCF, 0, 1.0f);
                    e[7] = e[7] + 1;
                }
                break;
            case 2:
                if (D_00810E74[0] & *(unsigned short *)0x70003B76) {
                    if (func_001787B0(e, 1) != 0) {
                        e[6] = 0x5A;
                        e[0x1F0] = 0x12;
                        func_001749A0(e, 0xD1, 0, 1.0f);
                    }
                } else {
                    func_001751A0(e);
                    if (*(int *)(e + 0x24C) != 3) {
                        func_001749A0(e, 0xD3, 0, 1.0f);
                        e[7] = e[7] + 1;
                    }
                }
                break;
            case 3:
                if (*(int *)(e + 0x200) & 0x1000) {
                    e[6] = 0;
                }
                break;
            }
        }
        break;
    case 80:
        func_0016A4B0(e);
        break;
    case 90:
        if (*(int *)(e + 0x200) & 0x1000) {
            e[6] = st + 1;
        }
        break;
    case 91:
        *(float *)0x70003A20 = 12.0f;
        e[6] = e[6] + 1;
        func_001749A0(e, 0xE5, 0, *(float *)0x70003A20);
        func_001FBD50(e, 0x187, 0, 300.0f);
        *(float *)(e + 0x2F4) = *(float *)(e + 0x2E0);
        *(float *)(e + 0x2F8) = *(float *)(e + 0x2E8);
        *(float *)(e + 0x258) = *(float *)(e + 0x2E4);
        *(short *)(e + 0x28) = float_to_int(*(float *)0x70003A20);
        *(float *)(e + 0x2E0) = (*(float *)(e + 0x2F4) - *(float *)(e + 0xB0)) / *(float *)0x70003A20;
        *(float *)(e + 0x2E8) = (*(float *)(e + 0x2F8) - *(float *)(e + 0xB8)) / *(float *)0x70003A20;
        *(float *)(e + 0x2E4) = (*(float *)(e + 0x258) - *(float *)(e + 0xB4)) / *(float *)0x70003A20;
        {
            float t = func_001B1470(*(float *)(e + 0x218) - *(float *)(e + 0xC4));
            *(float *)0x70003A24 = t;
            if (t >= 0.0f) {
                *(float *)(e + 0x26C) = t / 8.0f;
            } else {
                *(float *)(e + 0x26C) = -t / 8.0f;
            }
        }
        break;
    case 92:
        if (*(short *)(e + 0x28) == 0) {
            e[6] = st + 1;
            func_00182A70(e);
            *(float *)(e + 0xB0) = *(float *)(e + 0x2F4);
            *(float *)(e + 0xB8) = *(float *)(e + 0x2F8);
            *(float *)(e + 0xB4) = *(float *)(e + 0x258);
            *(float *)(e + 0xC4) = *(float *)(e + 0x218);
        } else {
            *(float *)(e + 0xB0) += *(float *)(e + 0x2E0);
            *(float *)(e + 0xB8) += *(float *)(e + 0x2E8);
            *(float *)(e + 0xB4) += *(float *)(e + 0x2E4);
            *(float *)(e + 0xC4) = func_001B12B0(*(float *)(e + 0x218), *(float *)(e + 0xC4), *(float *)(e + 0x26C));
            *(short *)(e + 0x28) = *(short *)(e + 0x28) - 1;
        }
        break;
    case 93:
        if (*(int *)(e + 0x200) & 0x1000) {
            func_00182A70(e);
            e[5] = 0xC;
            e[6] = 0;
            e[0x1F0] = 0x17;
            e[0xD] = 0;
            e[0x2F1] = 0;
            func_0017FC80(e, 16.0f);
        }
        break;
    }
}
