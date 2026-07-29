// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
// Player/actor state-machine tick, dispatched on the u8 state byte at +6.
// Sub-state byte at +0xD and the "outcome" byte at +0x1F1 select variants.
// NOTE (matching): the inner sub-state dispatches are ASCENDING if/else-if
// chains (bnez/bne), NOT switches -- mwcc lowers a switch to a DESCENDING
// beq chain, which is what the outer state dispatch uses.
// NOTE (matching): every float accumulate must be written as compound `+=`,
// not `x = x + y`; `+=` is what makes mwcc pair the FP scratch as $f0/$f1
// (add.s $f0,$f0,$f1) instead of $f0/$f2.
extern void anim_clip_arbiter(unsigned char *, int, float, float);
extern void build_trs_matrix(unsigned char *, unsigned char *, unsigned char *, unsigned char *);
extern int float_to_int(float);
extern void func_001026A0(unsigned char *, unsigned char *, void *);
extern float func_0011DE90(float);
extern float func_0011E2A8(float);
extern int func_001749A0(unsigned char *, int, int, float);
extern void func_00174FD0(unsigned char *);
extern unsigned char func_00178620(unsigned char *, int, int);
extern void func_00178B90(unsigned char *, int);
extern void func_00179150(unsigned char *);
extern int func_00179B90();
extern void func_0017DFB0(unsigned char *, int, float);
extern void func_0017E0D0(unsigned char *, int, float);
extern void func_0017E150(unsigned char *, int, int, float);
extern void func_0017E1D0(unsigned char *, int, float);
extern int func_0017F240(unsigned char *, int);
extern void func_0017FC80(unsigned char *, float);
extern void func_00182870(unsigned char *, int);
extern void func_00182A70();
extern int func_00188550(unsigned char *);
extern void func_001AEDE0(int, int);
extern void func_001AEE10(int, int);
extern void func_001B0460(int);
extern float func_001B12B0(float, float, float);
extern float func_001B1470(float);
extern int func_001C61D0(int, int);
extern int func_001FBD50(unsigned char *, int, int, float);

extern float D_002488AC[];
extern int D_00275498;
extern int D_00275B40;
extern short D_0028A9A0[];
extern unsigned char D_00810700[];
extern unsigned char D_00810702[];
extern unsigned short D_00810E74[];
extern int D_700038A0[];

void func_0016D130(unsigned char *arg0) {
    int st;
    int d;
    short t;
    float r;

    st = *(unsigned char *)(arg0 + 6);
    switch (st) {
    case 0:
        *(float *)(arg0 + 0x38) = 0.0f;
        d = *(unsigned char *)(arg0 + 0xD);
        if (d == 0) {
            *(unsigned char *)(arg0 + 6) = 3;
            func_001749A0(arg0, 0x152, 0, 8.0f);
            *(short *)(arg0 + 0x28) = 0x50;
            func_001FBD50(arg0, 0x122, 0, 300.0f);
        } else if (d == 1) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
            func_001749A0(arg0, 0x70, 0, 8.0f);
        } else if (d == 2) {
            int k;
            if (func_0017F240(arg0, 0) != 0) {
                return;
            }
            *(unsigned char *)(arg0 + 0x1F1) = 0;
            if (D_00810700[0] == 0) {
                *(unsigned char *)(arg0 + 6) = 0x14;
            } else {
                func_00174FD0(arg0);
                k = *(int *)(arg0 + 0x24C);
                if (k == 0) {
                    *(unsigned char *)(arg0 + 6) = 0xA;
                    *(unsigned char *)(arg0 + 0x1F1) = 1;
                } else if (k == 1) {
                    if (D_00810E74[0] & *(unsigned short *)0x70003B76) {
                        *(unsigned char *)(arg0 + 6) = 0x14;
                    }
                } else if (k == 2) {
                    *(unsigned char *)(arg0 + 0x2F1) = 0;
                    *(unsigned char *)(arg0 + 6) = 0x28;
                } else if (k == 3) {
                    *(unsigned char *)(arg0 + 0x2F1) = 1;
                    *(unsigned char *)(arg0 + 6) = 0x28;
                }
            }
        } else if (d == 3) {
            *(unsigned char *)(arg0 + 6) = 0x1E;
            func_001749A0(arg0, 0x70, 0, 8.0f);
        }
        *(unsigned char *)(arg0 + 7) = 0;
        break;
    case 1:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            *(unsigned char *)(arg0 + 6) = st + 1;
            func_00182870(arg0, 0);
            *(float *)0x70003A20 = (float)func_001C61D0(*(int *)(arg0 + 0x40), 0x79);
            anim_clip_arbiter(arg0, 0x79, 2.0f, *(float *)0x70003A20 - 53.0f);
            *(float *)(arg0 + 0x258) = *(float *)(arg0 + 0x254) - (*(float *)(arg0 + 0xB4) + D_002488AC[0]);
            *(float *)(arg0 + 0x2E4) = 0.6f;
            r = *(float *)(arg0 + 0x258) / *(float *)(arg0 + 0x2E4);
            *(float *)0x70003A20 = r;
            *(short *)(arg0 + 0x28) = float_to_int(r);
            *(float *)(arg0 + 0x254) = *(float *)(arg0 + 0x254) - D_002488AC[0];
            func_001FBD50(arg0, 0x12C, 0, 300.0f);
        }
        break;
    case 2:
        t = *(short *)(arg0 + 0x28);
        *(short *)(arg0 + 0x28) = t - 1;
        if (t == 0) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
            *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0x254);
            *(short *)(arg0 + 0x28) = 0x18;
        } else {
            *(float *)(arg0 + 0xB4) += *(float *)(arg0 + 0x2E4);
        }
        break;
    case 3:
        t = *(short *)(arg0 + 0x28);
        *(short *)(arg0 + 0x28) = t - 1;
        if (t == 0) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
            func_001FBD50(arg0, func_00179B90() + 0x13F, 0, 300.0f);
            func_001AEDE0(4, 0);
        }
        break;
    case 4:
        if (D_0028A9A0[0] == 2) {
            *(unsigned char *)(arg0 + 5) = 0x19;
            *(unsigned char *)(arg0 + 6) = 0;
            *(unsigned char *)(arg0 + 0x1F0) = 0x2D;
            if (*(unsigned char *)(arg0 + 0xD) == 1) {
                *(float *)(arg0 + 0xB4) = *(float *)(*(int *)(D_00275B40 + 4) + 0xC4);
                *(float *)(arg0 + 0xB0) += 7.5f * func_0011E2A8(*(float *)(arg0 + 0xC4));
                *(float *)(arg0 + 0xB8) += 7.5f * func_0011DE90(*(float *)(arg0 + 0xC4));
                build_trs_matrix(arg0 + 0xD0, arg0 + 0xB0, arg0 + 0xC0, arg0 + 0x60);
                func_00179150(arg0);
            } else {
                *(float *)(arg0 + 0xB0) = *(float *)(*(int *)(D_00275B40 + 4) + 0xC0);
                *(float *)(arg0 + 0xB8) = *(float *)(*(int *)(D_00275B40 + 4) + 0xC8);
            }
            *(unsigned char *)(arg0 + 1) = 0;
            func_001AEE10(4, 0);
        } else {
            *(int *)(arg0 + 0x204) = 0;
        }
        break;
    case 10:
        *(unsigned char *)(arg0 + 6) = st + 1;
        func_001749A0(arg0, 0x7C, 0, 5.0f);
        break;
    case 11:
        if (!(*(int *)(arg0 + 0x200) & 0x8000)) {
            *(unsigned char *)(arg0 + 6) = st + 1;
            func_001FBD50(arg0, 0x12C, 0, 300.0f);
        }
        break;
    case 12:
        if (*(float *)(arg0 + 0x3C) <= 18.0f) {
            *(unsigned char *)(arg0 + 6) = st + 1;
            func_001AEDE0(4, 0);
        }
        break;
    case 13:
        if (D_0028A9A0[0] == 2) {
            if (D_00810700[0] == 0) {
                *(unsigned char *)(arg0 + 5) = 0x1A;
                *(unsigned char *)(arg0 + 6) = 0;
                *(unsigned char *)(arg0 + 0x1F0) = 0x2E;
                *(unsigned char *)(arg0 + 0xD) = 0;
                *(float *)(arg0 + 0xB0) = 186.0f;
                *(float *)(arg0 + 0xB4) = -19.5f;
                *(float *)(arg0 + 0xB8) = -1463.5f;
                *(float *)(arg0 + 0xC4) = 3.1415927f;
                D_00810702[0] = 0xA;
                func_001B0460(1);
            } else {
                *(unsigned char *)(arg0 + 5) = 0x19;
                *(unsigned char *)(arg0 + 6) = 0;
                *(unsigned char *)(arg0 + 0x1F0) = 0x2D;
                *(unsigned char *)(arg0 + 1) = 0;
                *(float *)(arg0 + 0xB4) += 20.5f;
                build_trs_matrix(arg0 + 0xD0, arg0 + 0xB0, arg0 + 0xC0, arg0 + 0x60);
                *(float *)0x700038A0 = 0.0f;
                *(float *)0x700038A4 = 1.0f;
                *(float *)0x700038A8 = 5.0f;
                *(float *)0x700038AC = 1.0f;
                func_001026A0(arg0 + 0xB0, arg0 + 0xD0, D_700038A0);
                func_00179150(arg0);
            }
            func_001AEE10(4, 0);
        } else {
            *(int *)(arg0 + 0x204) = 0;
        }
        break;
    case 20:
        *(unsigned char *)(arg0 + 6) = st + 1;
        func_001749A0(arg0, 0x80, 0, 4.0f);
        *(float *)(arg0 + 0x21C) = 0.0f;
        *(float *)(arg0 + 0x38) = 0.0f;
        *(float *)(arg0 + 0x2E4) = 0.0f;
        *(float *)(arg0 + 0x2F4) = *(float *)(arg0 + 0xB4);
        break;
    case 21: {
        int fl;
        fl = *(int *)(arg0 + 0x200);
        if (fl & 0x1000) {
            *(unsigned char *)(arg0 + 5) = 7;
            *(unsigned char *)(arg0 + 6) = 0;
            *(unsigned char *)(arg0 + 0x1F0) = 0xD;
        } else if (!(fl & 0x8000)) {
            *(float *)(arg0 + 0x38) = *(float *)(*(int *)D_00275B40 + 8) - *(float *)(arg0 + 0x21C);
            *(float *)(arg0 + 0x21C) = *(float *)(*(int *)D_00275B40 + 8);
            func_00178B90(arg0, 1);
            *(float *)(arg0 + 0x2EC) = *(float *)(*(int *)D_00275B40 + 4) - *(float *)(arg0 + 0x2E4);
            *(float *)(arg0 + 0x2E4) = *(float *)(*(int *)D_00275B40 + 4);
            *(float *)(arg0 + 0xB4) += *(float *)(arg0 + 0x2EC);
        }
        break;
    }
    case 30:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            *(unsigned char *)(arg0 + 6) = st + 1;
            func_001749A0(arg0, 0x71, 0, 1.0f);
            *(unsigned char *)(arg0 + 0x25F) = 1;
            func_00182870(arg0, 0);
            *(float *)(arg0 + 0x254) = *(float *)(arg0 + 0x254) - 20.5f;
            *(float *)(arg0 + 0x2E4) = 0.6f;
            r = (*(float *)(arg0 + 0x254) - *(float *)(arg0 + 0xB4)) / *(float *)(arg0 + 0x2E4);
            *(float *)0x70003A20 = r;
            *(short *)(arg0 + 0x28) = float_to_int(r);
        }
        break;
    case 31:
        t = *(short *)(arg0 + 0x28);
        *(short *)(arg0 + 0x28) = t - 1;
        if (t == 0) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
            *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0x254);
            func_001749A0(arg0, 0x7A, 0, 1.0f);
        } else {
            *(float *)(arg0 + 0xB4) += *(float *)(arg0 + 0x2E4);
        }
        break;
    case 32:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            *(unsigned char *)(arg0 + 6) = 0;
            *(unsigned char *)(arg0 + 0x1F0) = 0x2C;
            *(unsigned char *)(arg0 + 0x1F1) = 0;
            *(unsigned char *)(arg0 + 0xD) = 2;
            func_001749A0(arg0, func_00188550(arg0), 0, 16.0f);
        }
        break;
    case 40:
        if (func_0017F240(arg0, 0) == 0) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
            func_0017DFB0(arg0, *(unsigned char *)(arg0 + 0x2F1), 8.0f);
        }
        break;
    case 41:
        if (func_0017F240(arg0, 0) == 0 && (*(int *)(arg0 + 0x200) & 0x1000)) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
            func_0017E0D0(arg0, *(unsigned char *)(arg0 + 0x2F1), 1.0f);
        }
        break;
    case 42:
        if (func_0017F240(arg0, 0) == 0) {
            int k;
            int m;
            func_00174FD0(arg0);
            k = *(unsigned char *)(arg0 + 0x2F1);
            m = *(int *)(arg0 + 0x24C);
            if (m != (&D_00275498)[k]) {
                *(unsigned char *)(arg0 + 6) = 0x31;
                func_0017E150(arg0, *(unsigned char *)(arg0 + 0x2F1), m, 1.0f);
            } else if (D_00810E74[0] & *(unsigned short *)0x70003B76) {
                unsigned char tv;
                *(unsigned char *)(arg0 + 0x1F1) = func_00178620(arg0, k & 0xFF, m);
                tv = *(unsigned char *)(arg0 + 0x1F1);
                if (tv == 1 || tv == 3) {
                    *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
                    *(unsigned char *)(arg0 + 0xD) = 4;
                    func_0017E1D0(arg0, *(unsigned char *)(arg0 + 0x2F1), 1.0f);
                    *(unsigned char *)(arg0 + 0x1F0) = 0x12;
                }
            }
        }
        break;
    case 43:
        if (*(float *)(arg0 + 0x3C) <= 6.0f) {
            *(unsigned char *)(arg0 + 6) = st + 1;
            func_001FBD50(arg0, 0x187, 0, 300.0f);
            *(float *)(arg0 + 0x2F4) = *(float *)(arg0 + 0x2E0);
            *(float *)(arg0 + 0x2F8) = *(float *)(arg0 + 0x2E8);
            *(float *)(arg0 + 0x258) = *(float *)(arg0 + 0x2E4);
            *(short *)(arg0 + 0x28) = 8;
            *(float *)(arg0 + 0x2E0) = (*(float *)(arg0 + 0x2F4) - *(float *)(arg0 + 0xB0)) / 8.0f;
            *(float *)(arg0 + 0x2E8) = (*(float *)(arg0 + 0x2F8) - *(float *)(arg0 + 0xB8)) / 8.0f;
            *(float *)(arg0 + 0x2E4) = (*(float *)(arg0 + 0x258) - *(float *)(arg0 + 0xB4)) / 8.0f;
            r = func_001B1470(*(float *)(arg0 + 0x218) - *(float *)(arg0 + 0xC4));
            *(float *)0x70003A20 = r;
            if (!(r < 0.0f)) {
                *(float *)(arg0 + 0x26C) = r / 8.0f;
            } else {
                *(float *)(arg0 + 0x26C) = -r / 8.0f;
            }
            if (*(unsigned char *)(arg0 + 0x1F1) == 1) {
                func_001749A0(arg0, 0xE5, 0, 8.0f);
            } else {
                func_001749A0(arg0, 0x7A, 0, 8.0f);
            }
        }
        break;
    case 44:
        if (*(short *)(arg0 + 0x28) == 0) {
            *(unsigned char *)(arg0 + 6) = st + 1;
            *(float *)(arg0 + 0xB0) = *(float *)(arg0 + 0x2F4);
            *(float *)(arg0 + 0xB8) = *(float *)(arg0 + 0x2F8);
            *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0x258);
            *(float *)(arg0 + 0xC4) = *(float *)(arg0 + 0x218);
            if (*(unsigned char *)(arg0 + 0x1F1) == 1) {
                func_00182A70();
            } else {
                func_001FBD50(arg0, 0xFF, 0, 300.0f);
            }
        } else {
            *(float *)(arg0 + 0xB0) += *(float *)(arg0 + 0x2E0);
            *(float *)(arg0 + 0xB8) += *(float *)(arg0 + 0x2E8);
            *(float *)(arg0 + 0xB4) += *(float *)(arg0 + 0x2E4);
            *(float *)(arg0 + 0xC4) = func_001B12B0(*(float *)(arg0 + 0x218), *(float *)(arg0 + 0xC4), *(float *)(arg0 + 0x26C));
            *(short *)(arg0 + 0x28) = *(short *)(arg0 + 0x28) - 1;
        }
        break;
    case 45:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            d = *(unsigned char *)(arg0 + 0x1F1);
            if (d == 1) {
                func_00182A70();
                *(unsigned char *)(arg0 + 5) = 0xC;
                *(unsigned char *)(arg0 + 6) = 0;
                *(unsigned char *)(arg0 + 0x1F0) = 0x17;
                *(unsigned char *)(arg0 + 0xD) = 1;
                *(unsigned char *)(arg0 + 0x2F1) = 0;
                func_0017FC80(arg0, 16.0f);
            } else if (d == 3) {
                *(unsigned char *)(arg0 + 5) = 9;
                *(unsigned char *)(arg0 + 6) = 0;
                *(unsigned char *)(arg0 + 0x1F0) = 0x10;
                *(unsigned char *)(arg0 + 0xD) = 1;
                func_001749A0(arg0, func_00188550(arg0), 0, 16.0f);
            }
        }
        break;
    case 49:
        if (func_0017F240(arg0, 0) == 0 && (*(int *)(arg0 + 0x200) & 0x1000)) {
            *(unsigned char *)(arg0 + 6) = 0;
            func_001749A0(arg0, func_00188550(arg0), 0, 16.0f);
        }
        break;
    }
}
