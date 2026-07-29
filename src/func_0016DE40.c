// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// Player/entity state-machine tick, dispatched on the sub-state byte at p+6.
// D_00810700 = current level/area id, D_00810701 = sub-area, D_00810702 = a
// "where am I" zone code written when the entity's world position falls inside
// one of the hard-coded AABBs below; p+0x302 is the "zone valid" flag.
// D_00275B40 is a gp-relative pointer-to-pointer to the global frame timer.
// NOTE: -sdatathreshold 4 is required so D_00275B40 is reached %gp_rel; the
// other small externs are declared as incomplete arrays so they stay %hi/%lo.

extern char D_008106BE[];
extern unsigned char D_00810700[];
extern unsigned char D_00810701[];
extern char D_00810702[];
extern unsigned short D_00810E74[];
extern short D_0028A9A0[];
extern float D_002488B0[];
extern float **D_00275B40;

extern int func_001749A0(char *p, int clip, int a, float b);
extern int func_00174FD0(char *p);
extern int func_00176F90(char *p);
extern int func_00179010(char *p);
extern int func_00179150(char *p);
extern int func_00179450(char *p, float *pos);
extern int func_00179910(char *p);
extern int func_00179B90(void);
extern int func_001823E0(char *p);
extern int func_00184BA0(char *p);
extern int func_00188610(char *p);
extern int func_00199DB0(void *v);
extern void func_001AEDE0(int a, int b);
extern void func_001AEE10(int a, int b);
extern void func_001B0460(int a);
extern float func_001B12B0(float a, float b, float step);
extern float func_001B1470(float a);
extern int func_001FB9F0(int a, int b, int c, int d);

void func_0016DE40(char *p)
{
    unsigned char st;
    unsigned char sub;
    int kind;
    short tmr;
    float x;
    float y;
    float z;
    float d;

    *(char *)(p + 1) = 0;
    st = *(unsigned char *)(p + 6);
    switch (st) {
    case 0:
        D_008106BE[0] = 2;
        *(unsigned char *)(p + 0x302) = 0;
        if (D_00810700[0] == 0 && !(*(float *)(p + 0xB8) <= -1470.0f)) {
            *(char *)(p + 0xD) = 2;
            *(char *)(p + 5) = 0x1A;
            *(unsigned char *)(p + 6) = 0;
            *(char *)(p + 0x1F0) = 0x2E;
            *(float *)(p + 0xB0) = 185.8f;
            *(float *)(p + 0xB8) = -1450.0f;
            *(float *)(p + 0xB4) += -0.2f;
            D_00810702[0] = 5;
            func_001B0460(1);
            return;
        }
        *(unsigned char *)(p + 6) = *(unsigned char *)(p + 6) + 1;
        *(char *)(p + 7) = 0;
        *(int *)(p + 0x38) = 0;
        *(char *)(p + 0x1F1) = 0;
        func_001749A0(p, func_00188610(p), 0, 0.0f);
        for (;;) {
            *(float *)(p + 0xB4) = *(float *)(p + 0xB4) - 1.0f;
            if (func_00179010(p) != 0) {
                break;
            }
        }
        break;

    case 1:
        if (D_0028A9A0[0] == 0) {
            *(unsigned char *)(p + 6) = 0xA;
        }
        break;

    case 0xA:
        if (func_001823E0(p) != 0) {
            break;
        }
        if ((D_00810E74[0] & *(unsigned short *)0x70003B76) != 0 && func_00184BA0(p) != 0) {
            break;
        }
        func_001749A0(p, func_00188610(p), 0, 1.0f);
        func_00174FD0(p);
        kind = *(int *)(p + 0x24C);
        if (kind == 0) {
            *(unsigned char *)(p + 6) = 0x14;
            break;
        }
        if (kind == 1) {
            *(unsigned char *)(p + 6) = 0x1E;
            break;
        }
        if ((unsigned int)(kind - 2) < 2) {
            if (func_00176F90(p) == 0x1F) {
                func_00199DB0(p + 0x290);
                *(float *)(p + 0xB0) = *(float *)(p + 0x290);
                *(float *)(p + 0xB8) = *(float *)(p + 0x298);
            }
            *(unsigned char *)(p + 6) = 0x28;
        }
        break;

    case 0x14:
        if (func_001823E0(p) != 0) {
            break;
        }
        if ((D_00810E74[0] & *(unsigned short *)0x70003B76) != 0 && func_00184BA0(p) != 0) {
            break;
        }
        *(unsigned char *)(p + 6) = *(unsigned char *)(p + 6) + 1;
        sub = *(unsigned char *)(p + 0x23F);
        if (sub == 1) {
            *(float *)(p + 0x2F4) = 0.5f;
        } else if (sub == 2) {
            *(float *)(p + 0x2F4) = 0.7f;
        } else {
            *(float *)(p + 0x2F4) = 1.0f;
        }
        func_001749A0(p, 0x14B, 0, 1.0f);
        func_001FB9F0(func_00179B90() + 0x13F, 0x1000, 0x1000, 0x1000);
        *(int *)(p + 0x38) = 0;
        *(int *)(p + 0x21C) = 0;
        break;

    case 0x15:
        if (func_001823E0(p) != 0) {
            break;
        }
        if ((D_00810E74[0] & *(unsigned short *)0x70003B76) != 0 && func_00184BA0(p) != 0) {
            break;
        }
        if ((*(int *)(p + 0x200) & 0x1000) != 0) {
            *(unsigned char *)(p + 6) = 0xA;
            break;
        }
        *(float *)(p + 0x204) = *(float *)(p + 0x2F4);
        *(float *)(p + 0x38) = **D_00275B40 - *(float *)(p + 0x21C);
        *(float *)(p + 0x21C) = **D_00275B40;
        func_00179150(p);
        *(float *)(p + 0xB4) += -0.2f;
        func_00179010(p);
        *(unsigned char *)(p + 0x302) = 0;
        if (*(unsigned char *)(p + 0xA) == 0) {
            if (D_00810700[0] == 3) {
                x = *(float *)(p + 0xB0);
                if (!(x < 648.0f) && x <= 668.0f) {
                    y = *(float *)(p + 0xB4);
                    if (!(y < 45.0f) && y <= 65.0f) {
                        z = *(float *)(p + 0xB8);
                        if (!(z < 704.0f) && z <= 724.0f) {
                            D_00810702[0] = 7;
                            *(unsigned char *)(p + 0x302) = 1;
                            goto move;
                        }
                    }
                }
                if (!(x < 624.0f) && x <= 644.0f) {
                    y = *(float *)(p + 0xB4);
                    if (!(y < 45.0f) && y <= 65.0f) {
                        z = *(float *)(p + 0xB8);
                        if (!(z < 792.0f) && z <= 812.0f) {
                            D_00810702[0] = 4;
                            *(unsigned char *)(p + 0x302) = 1;
                            goto move;
                        }
                    }
                }
                if (!(x < 547.0f) && x <= 567.0f) {
                    x = *(float *)(p + 0xB4);
                    if (!(x < 45.0f) && x <= 65.0f) {
                        x = *(float *)(p + 0xB8);
                        if (!(x < 705.0f) && x <= 725.0f) {
                            D_00810702[0] = 2;
                            *(unsigned char *)(p + 0x302) = 1;
                            goto move;
                        }
                    }
                }
                D_00810702[0] = 0;
                *(unsigned char *)(p + 0x302) = 1;
            } else if (D_00810700[0] == 8 && D_00810701[0] == 3) {
                    x = *(float *)(p + 0xB0);
                    if (!(x < 110.5f) && x <= 135.0f) {
                        z = *(float *)(p + 0xB8);
                        if (!(z < 146.0f) && z <= 176.0f) {
                            *(float *)(p + 0xB0) = 123.5f;
                            *(float *)(p + 0xB8) = 156.4f;
                            D_00810702[0] = 0;
                            goto move;
                        }
                    }
                    if (!(x < 122.6f) && x <= 142.6f) {
                        x = *(float *)(p + 0xB8);
                        if (!(x < 87.0f) && x <= 107.0f) {
                            D_00810702[0] = 2;
                            *(unsigned char *)(p + 0x302) = 1;
                        }
                    }
            } else if (D_00810700[0] == 0x13 && D_00810701[0] == 0) {
                    x = *(float *)(p + 0xB0);
                    if (!(x < 710.0f) && x <= 730.0f) {
                        x = *(float *)(p + 0xB8);
                        if (!(x < 960.0f) && x <= 1160.0f) {
                            D_00810702[0] = 8;
                            *(unsigned char *)(p + 0x302) = 1;
                        }
                    }
            }
        move:
            if (func_00179450(p, (float *)(p + 0xB0)) != 0) {
                d = *(float *)(p + 0x258);
                if (d < -D_002488B0[0]) {
                    *(char *)(p + 0xD) = 2;
                } else if (!(d <= -4.01f)) {
                    *(char *)(p + 0xD) = 0;
                    for (;;) {
                        *(float *)(p + 0xB4) = *(float *)(p + 0xB4) - 1.0f;
                        if (func_00179010(p) != 0) {
                            break;
                        }
                    }
                } else {
                    *(char *)(p + 0xD) = 1;
                }
                *(unsigned char *)(p + 6) = *(unsigned char *)(p + 6) + 1;
            }
        } else if (*(unsigned char *)(p + 0x23B) == 0x37) {
            *(char *)(p + 0xD) = 0;
            *(unsigned char *)(p + 6) = *(unsigned char *)(p + 6) + 1;
            if (D_00810700[0] == 0x13) {
                y = *(float *)(p + 0xB4);
                if (!(y < 230.0f) && y <= 250.0f) {
                    y = *(float *)(p + 0xB0);
                    if (!(y < 693.0f) && y <= 713.0f) {
                        y = *(float *)(p + 0xB8);
                        if (!(y < 1209.0f) && y <= 1229.0f) {
                            D_00810702[0] = 9;
                            *(unsigned char *)(p + 0x302) = 1;
                        }
                    }
                }
            } else if (D_00810700[0] == 3) {
                D_00810702[0] = 0;
                *(unsigned char *)(p + 0x302) = 1;
            } else if (D_00810700[0] == 0) {
                y = *(float *)(p + 0xB4);
                if (y <= -85.0f) {
                    D_00810702[0] = 8;
                    *(unsigned char *)(p + 0x302) = 1;
                } else if (y <= -65.0f) {
                    D_00810702[0] = 9;
                    *(unsigned char *)(p + 0x302) = 1;
                }
            }
        }
        if (func_00179910(p) != 0) {
            *(unsigned char *)(p + 6) = 0x63;
            func_001AEDE0(4, 0);
            return;
        }
        break;

    case 0x16:
        *(unsigned char *)(p + 6) = st + 1;
        func_001AEDE0(4, 0);
        return;

    case 0x17:
        if (D_0028A9A0[0] == 2) {
            *(char *)(p + 5) = 0x1A;
            *(unsigned char *)(p + 6) = 0;
            *(char *)(p + 0x1F0) = 0x2E;
            func_001AEE10(4, 0);
            if (*(unsigned char *)(p + 0x302) != 0) {
                func_001B0460(1);
                return;
            }
        }
        break;

    case 0x1E:
        if (func_001823E0(p) != 0) {
            break;
        }
        *(unsigned char *)(p + 6) = *(unsigned char *)(p + 6) + 1;
        *(float *)(p + 0x26C) = func_001B1470(3.1415927f + *(float *)(p + 0xC4));
        return;

    case 0x1F:
        if (func_001823E0(p) != 0) {
            break;
        }
        *(float *)(p + 0xC4) = func_001B12B0(*(float *)(p + 0x26C), *(float *)(p + 0xC4), 0.06981317f);
        if (*(float *)(p + 0x26C) == *(float *)(p + 0xC4)) {
            *(unsigned char *)(p + 6) = 0x32;
        }
        break;

    case 0x28:
        if (func_001823E0(p) != 0) {
            break;
        }
        *(unsigned char *)(p + 6) = *(unsigned char *)(p + 6) + 1;
        if (*(int *)(p + 0x24C) == 2) {
            *(float *)(p + 0x26C) = func_001B1470(*(float *)(p + 0xC4) - 1.5707964f);
        } else {
            *(float *)(p + 0x26C) = func_001B1470(1.5707964f + *(float *)(p + 0xC4));
        }
        /* fallthrough */
    case 0x29:
        if (func_001823E0(p) != 0) {
            break;
        }
        *(float *)(p + 0xC4) = func_001B12B0(*(float *)(p + 0x26C), *(float *)(p + 0xC4), 0.06981317f);
        if (*(float *)(p + 0x26C) == *(float *)(p + 0xC4)) {
            *(unsigned char *)(p + 6) = 0x32;
        }
        break;

    case 0x32:
        if (func_001823E0(p) != 0) {
            break;
        }
        *(unsigned char *)(p + 6) = *(unsigned char *)(p + 6) + 1;
        *(short *)(p + 0x28) = 0x10;
        /* fallthrough */
    case 0x33:
        if (func_001823E0(p) != 0) {
            break;
        }
        tmr = *(short *)(p + 0x28);
        *(short *)(p + 0x28) = tmr - 1;
        if (tmr == 0) {
            *(unsigned char *)(p + 6) = 0xA;
        }
        break;

    case 0x63:
        break;
    }
}
