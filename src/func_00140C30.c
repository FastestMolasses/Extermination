// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Two-state actor step keyed on the low-7-bit state field arg1[0x81] (bit 0x80 is a
// separate flag). Every tick (unless state==2) decays arg1.0x48 by 0.1f and folds it
// into arg0.0xB4. If the 0x80 flag is clear and arg1.0x5C <= 12.5f and a proximity probe
// func_001B1560(arg0, &D_00810360, ~1.1345f) hits, latch arg0.0x52 bit 0 and return.
// Otherwise dispatch on state: state 0 integrates position/velocity floats
// (arg1+0x4C/0x50, arg0+0xB0/0xB8) using sin (func_0011E2A8) / cos (func_0011DE90) of the
// heading arg0.0xC4. State 1 early-outs if arg0.0x2C's sign bit (0x8000) is set, else runs
// an inner switch on the short arg0.0x2C selecting one of eight interpolation tables
// (D_002451E0/245250/2452C0/245380/245400/2454C0/245550/245610; default clears
// arg1[0x81]), then a func_001C6160-driven loop accumulating weighted sin/cos
// contributions from the selected table.

extern int float_to_int(float);
extern float func_0011DE90(float);
extern void func_0011E0A8(float *out, float v);
extern float func_0011E2A8(float);
extern int func_001B1560(unsigned char *p, void *v, float r);
extern int func_001C6160(unsigned char *p);

extern float D_002451E0[];
extern float D_00245250[];
extern float D_002452C0[];
extern float D_00245380[];
extern float D_00245400[];
extern float D_002454C0[];
extern float D_00245550[];
extern float D_00245610[];
extern unsigned char D_00810360;

void func_00140C30(unsigned char *arg0, unsigned char *arg1) {
    int st;
    short kind;
    float *tbl;
    float sp68;
    float sp6C;
    float f20;

    if ((*(signed char *)(arg1 + 0x81) & 0x7F) != 2) {
        *(float *)(arg1 + 0x48) += -0.1f;
        *(float *)(arg0 + 0xB4) += *(float *)(arg1 + 0x48);
    }

    if (!(*(signed char *)(arg1 + 0x81) & 0x80) && *(float *)(arg1 + 0x5C) <= 12.5f &&
        func_001B1560(arg0, &D_00810360, 1.1344640254974365f) != 0) {
        *(unsigned short *)(arg0 + 0x52) |= 1;
        return;
    }

    st = *(signed char *)(arg1 + 0x81) & 0x7F;
    switch (st) {
    case 0:
        *(float *)(arg1 + 0x4C) += *(float *)(arg1 + 0x50);
        *(float *)(arg0 + 0xB0) += *(float *)(arg1 + 0x4C) * func_0011E2A8(*(float *)(arg0 + 0xC4));
        *(float *)(arg0 + 0xB8) += *(float *)(arg1 + 0x4C) * func_0011DE90(*(float *)(arg0 + 0xC4));
        break;
    case 1:
        kind = *(short *)(arg0 + 0x2C);
        if (kind & 0x8000) {
            return;
        }
        switch (kind) {
        case 3:
            tbl = D_002451E0;
            break;
        case 4:
            tbl = D_00245250;
            break;
        case 5:
            tbl = D_002452C0;
            break;
        case 8:
            tbl = D_00245380;
            break;
        case 19:
            tbl = D_00245400;
            break;
        case 21:
            tbl = D_002454C0;
            break;
        case 27:
            tbl = D_00245550;
            break;
        case 35:
            tbl = D_00245610;
            break;
        default:
            arg1[0x81] = 0;
            return;
        }
        f20 = (float)func_001C6160(arg0) - *(float *)(arg0 + 0x3C);
        func_0011E0A8(&sp68, f20);
        func_0011E0A8(&sp6C, 1.0f + (f20 - *(float *)(arg1 + 0x40)));
        sp68 = sp68 - sp6C;
        if (sp68 < 0.0f) {
            return;
        }
        do {
            float *cell = tbl + float_to_int(f20);
            *(float *)(arg1 + 0x4C) = *cell;
            *(float *)(arg0 + 0xB0) += *cell * func_0011E2A8(*(float *)(arg0 + 0xC4));
            *(float *)(arg0 + 0xB8) += *cell * func_0011DE90(*(float *)(arg0 + 0xC4));
            f20 -= 1.0f;
            if (f20 < 0.0f) {
                f20 = (float)(func_001C6160(arg0) - 1);
            }
            sp68 -= 1.0f;
        } while (!(sp68 <= 0.0f));
        break;
    }
}
