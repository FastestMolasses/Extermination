// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 81.40% NEARMISS).
// Fixes read from the original instructions:
//  - The default (no-table) case leaves through a jump to the function's end
//    (`goto done`), not a `return`: with `return` mwcc lays the default block
//    directly after the compare chain and fills every case compare's delay
//    slot with a branch-likely table-address load; the original keeps plain
//    compares with empty slots and places the default block after the case
//    blocks, ahead of the integration loop.
//
// Heading-integration table dispatch: decays the arg1+0x38 timer into the
// arg0+0xB4 accumulator, gates on func_001000E0/func_00128350 and the
// func_001B1560 proximity probe (sets arg0+0x52 bit 0 and stops), then a
// 22-way switch on arg0+0x2C selects a per-state interpolation-weight table.
// Unlisted states just drift by the arg1+0x3C weight along arg0+0xC4. Listed
// states run a func_001C6160-clocked do-while that steps through the table,
// wrapping the index when it goes negative.

extern int float_to_int(float);
extern int func_001000E0(int, double);
extern float func_0011DE90(float);
extern void func_0011E0A8(float *out, float v);
extern float func_0011E2A8(float);
extern int func_00128350(float);
extern int func_001B1560(unsigned char *p, void *v, float r);
extern int func_001C6160(unsigned char *p);

extern float D_002433B0[];
extern float D_00243540[];
extern float D_00243660[];
extern float D_00243710[];
extern float D_002437D0[];
extern float D_002438E0[];
extern float D_002439A0[];
extern float D_00243A60[];
extern float D_00243B90[];
extern float D_00243CB0[];
extern float D_00243DA0[];
extern float D_00243E60[];
extern float D_00243FF0[];
extern float D_00244130[];
extern float D_002441B0[];
extern float D_002442D0[];
extern float D_00244490[];
extern float D_00244580[];
extern float D_00244800[];
extern float D_00244A50[];
extern float D_00244AF0[];
extern float D_00244C30[];
extern unsigned char D_00810360;

void func_00132490(unsigned char *arg0, unsigned char *arg1) {
    float *tbl;
    float sp58;
    float sp5C;
    float f20;

    *(float *)(arg1 + 0x38) += -0.1f;
    *(float *)(arg0 + 0xB4) += *(float *)(arg1 + 0x38);

    if (func_001000E0(func_00128350(*(float *)(arg1 + 0x44)), 11.5) != 0 &&
        func_001B1560(arg0, &D_00810360, 1.1344640254974365f) != 0) {
        *(unsigned short *)(arg0 + 0x52) |= 1;
        return;
    }

    switch (*(short *)(arg0 + 0x2C)) {
    case 2:
        tbl = D_002433B0;
        break;
    case 7:
        tbl = D_00243540;
        break;
    case 8:
        tbl = D_00243660;
        break;
    case 9:
        tbl = D_00243710;
        break;
    case 10:
        tbl = D_002437D0;
        break;
    case 14:
        tbl = D_002438E0;
        break;
    case 15:
        tbl = D_002439A0;
        break;
    case 16:
        tbl = D_00243A60;
        break;
    case 17:
        tbl = D_00243B90;
        break;
    case 18:
        tbl = D_00243CB0;
        break;
    case 19:
        tbl = D_00243DA0;
        break;
    case 20:
        tbl = D_00243E60;
        break;
    case 23:
        tbl = D_00243FF0;
        break;
    case 24:
        tbl = D_00244130;
        break;
    case 25:
        tbl = D_002441B0;
        break;
    case 26:
        tbl = D_002442D0;
        break;
    case 27:
        tbl = D_00244490;
        break;
    case 28:
        tbl = D_00244580;
        break;
    case 29:
        tbl = D_00244800;
        break;
    case 32:
        tbl = D_00244A50;
        break;
    case 33:
        tbl = D_00244AF0;
        break;
    case 40:
        tbl = D_00244C30;
        break;
    default:
        if (*(float *)(arg1 + 0x3C) != 0.0f) {
            *(float *)(arg0 + 0xB0) += *(float *)(arg1 + 0x3C) * func_0011E2A8(*(float *)(arg0 + 0xC4));
            *(float *)(arg0 + 0xB8) += *(float *)(arg1 + 0x3C) * func_0011DE90(*(float *)(arg0 + 0xC4));
        }
        goto done;
    }

    f20 = (float)func_001C6160(arg0) - *(float *)(arg0 + 0x3C);
    func_0011E0A8(&sp58, f20);
    func_0011E0A8(&sp5C, 1.0f + (f20 - *(float *)(arg1 + 0x34)));
    sp58 = sp58 - sp5C;
    if (sp58 < 0.0f) {
        return;
    }
    do {
        *(float *)(arg1 + 0x3C) = tbl[float_to_int(f20)];
        *(float *)(arg0 + 0xB0) += *(float *)(arg1 + 0x3C) * func_0011E2A8(*(float *)(arg0 + 0xC4));
        *(float *)(arg0 + 0xB8) += *(float *)(arg1 + 0x3C) * func_0011DE90(*(float *)(arg0 + 0xC4));
        f20 -= 1.0f;
        if (f20 < 0.0f) {
            f20 = (float)(func_001C6160(arg0) - 1);
        }
        sp58 -= 1.0f;
    } while (!(sp58 <= 0.0f));
done:
    ;
}
