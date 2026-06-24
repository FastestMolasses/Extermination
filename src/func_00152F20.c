// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Two-state actor step keyed on the signed byte arg1[0x53]. State 0 integrates
// position/velocity floats (arg1+0x30/0x38, arg0+0xB0/0xB4/0xB8) using sin
// (func_0011E2A8) and cos (func_0011DE90) of the heading arg0[0xC4]. State 1 runs
// an inner switch on the short arg0[0x2C] (early-out if 0x8000 set) selecting one
// of four interpolation tables (D_00246040/230/410/630), then a func_001C6160-driven
// loop accumulating weighted sin/cos contributions from the selected table.

extern int float_to_int(float);
extern float func_0011DE90(float);
extern void func_0011E0A8(float *out, float v);
extern float func_0011E2A8(float);
extern int func_001C6160(unsigned char *p);

extern int D_00246040[];
extern int D_00246230[];
extern int D_00246410[];
extern int D_00246630[];

void func_00152F20(unsigned char *arg0, unsigned char *arg1) {
    float sp58;
    float sp5C;
    float *tbl;
    float f20;
    float f21;
    signed char st;
    short kind;

    st = (signed char)arg1[0x53];
    switch (st) {
    case 0:
        *(float *)(arg1 + 0x30) += *(float *)(arg1 + 0x34);
        *(float *)(arg1 + 0x38) += *(float *)(arg1 + 0x3C);
        *(float *)(arg0 + 0xB4) += *(float *)(arg1 + 0x30);
        *(float *)(arg0 + 0xB0) += *(float *)(arg1 + 0x38) * func_0011E2A8(*(float *)(arg0 + 0xC4));
        *(float *)(arg0 + 0xB8) += *(float *)(arg1 + 0x38) * func_0011DE90(*(float *)(arg0 + 0xC4));
        break;
    case 1:
        kind = *(short *)(arg0 + 0x2C);
        if (kind & 0x8000) {
            return;
        }
        switch (kind) {
        case 0:
            tbl = (float *)D_00246040;
            break;
        case 2:
            tbl = (float *)D_00246230;
            break;
        case 3:
            tbl = (float *)D_00246410;
            break;
        case 4:
            tbl = (float *)D_00246630;
            break;
        default:
            arg1[0x53] = 0;
            return;
        }
        f20 = (float)func_001C6160(arg0) - *(float *)(arg0 + 0x3C);
        func_0011E0A8(&sp58, f20);
        func_0011E0A8(&sp5C, 1.0f + (f20 - *(float *)(arg1 + 0x28)));
        sp58 = sp58 - sp5C;
        if (sp58 < 0.0f) {
            return;
        }
        do {
            float *cell;
            f21 = func_0011E2A8(*(float *)(arg0 + 0xC4));
            cell = tbl + float_to_int(f20);
            *(float *)(arg0 + 0xB0) += *cell * f21;
            *(float *)(arg0 + 0xB8) = *(float *)(arg0 + 0xB8) + (*cell * func_0011DE90(*(float *)(arg0 + 0xC4)));
            f20 -= 1.0f;
            if (f20 < 0.0f) {
                f20 = (float)(func_001C6160(arg0) - 1);
            }
            sp58 = sp58 - 1.0f;
        } while (!(sp58 <= 0.0f));
        break;
    }
}
