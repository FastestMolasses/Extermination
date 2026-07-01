// NEARMISS func_001C40B0  (vram 0x001C40B0, 0x670 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 89.85% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// $at-vs-GPR compare, shared-epilogue edge case: every clamp-to-99 test's slti result lives in a named GPR ($v0) here vs the target's $at, despite being a genuinely pure branch to the same shared function-end. Inverting polarity and forcing an explicit reload did not change it; looks like a build-s...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Stat/need adjuster: arg0 selects which stat table entry (D_00810C64[arg0])
// and which linked meter to bump; arg1 is the signed delta. Most cases add
// arg1 (or a scaled multiple) into a linked 16-bit meter and clamp it to
// [.., 99]; a few cases (16/27/28/29) additionally track a running "peak"
// value (D_00810CB7) used to rescale D_00810CB2. Case 15 is a raw byte
// store (no meter). Cases 12/13/14 only bump the fatigue meter D_00810CB0
// when three gate flags (D_00810C70/71/72) are all set. The default case
// just clamps the stat-table byte itself to 99.
//
// Case labels are declared in ASCENDING order (0x10..0xE) so mwcc's switch
// lowering tests them in the target's DESCENDING dispatch order (E,D,C,4,3,
// 2,1,F,1D,1C,1B,16,15,14,13,12,11,10,default).
extern unsigned char D_00810C64[0x200];
extern unsigned char D_00810C62;
extern unsigned char D_00810C63;
extern unsigned char D_00810C70;
extern unsigned char D_00810C71;
extern unsigned char D_00810C72;
extern short D_00810CA8;
extern short D_00810CAA;
extern short D_00810CAC;
extern short D_00810CAE;
extern short D_00810CB0;
extern short D_00810CB2;
extern unsigned char D_00810CB7;
extern short D_00810CB4;

int func_001C40B0(int arg0, int arg1) {
    switch (arg0) {
    case 0x10:
        D_00810C64[arg0] += arg1;
        D_00810C63 += arg1;
        D_00810CB4 += arg1 * 0x1E;
        if (D_00810C62 == 0) {
            D_00810C62 = 0x1E;
        }
        if (D_00810C63 >= 0x63) {
            D_00810CB4 -= (D_00810C63 - 0x62) * 0x1E;
            D_00810C63 = 0x62;
            D_00810C64[arg0] = 0x62;
        }
        break;

    case 0x11:
        D_00810CA8 += arg1 * 6;
        if (D_00810CA8 >= 0x64) {
            D_00810CA8 = 0x63;
        }
        break;

    case 0x12:
        D_00810CA8 += arg1 * 0xC;
        if (D_00810CA8 >= 0x64) {
            D_00810CA8 = 0x63;
        }
        break;

    case 0x13:
        D_00810CAA += arg1 * 6;
        if (D_00810CAA >= 0x64) {
            D_00810CAA = 0x63;
        }
        break;

    case 0x14:
        D_00810CAA += arg1 * 0xC;
        if (D_00810CAA >= 0x64) {
            D_00810CAA = 0x63;
        }
        break;

    case 0x15:
        D_00810CAC += arg1;
        if (D_00810CAC >= 0x64) {
            D_00810CAC = 0x63;
            D_00810CAE = 0;
        }
        break;

    case 0x16:
        D_00810CB0 += arg1;
        if (D_00810CB0 >= 0x64) {
            D_00810CB0 = 0x63;
        }
        break;

    case 0x1B:
        D_00810C64[arg0] += arg1;
        D_00810CB2 += arg1 * 0xC;
        if (D_00810CB7 < 0xC) {
            D_00810CB7 = 0xC;
        }
        if (D_00810CB7 < D_00810CB2) {
            D_00810CB2 = D_00810CB7;
        }
        break;

    case 0x1C:
        D_00810C64[arg0] += arg1;
        D_00810CB2 += arg1 * 0x24;
        if (D_00810CB7 < 0x24) {
            D_00810CB7 = 0x24;
        }
        if (D_00810CB7 < D_00810CB2) {
            D_00810CB2 = D_00810CB7;
        }
        break;

    case 0x1D:
        D_00810C64[arg0] += arg1;
        D_00810CB2 += arg1 * 0x30;
        if (D_00810CB7 < 0x30) {
            D_00810CB7 = 0x30;
        }
        if (D_00810CB7 < D_00810CB2) {
            D_00810CB2 = D_00810CB7;
        }
        break;

    case 0xF:
        D_00810C64[arg0] = arg1;
        break;

    case 1:
        D_00810C64[arg0] += arg1;
        D_00810CA8 += 6;
        if (D_00810CA8 >= 0x64) {
            D_00810CA8 = 0x63;
        }
        break;

    case 2:
    case 3:
        D_00810C64[arg0] += arg1;
        D_00810CAA += 6;
        if (D_00810CAA >= 0x64) {
            D_00810CAA = 0x63;
        }
        break;

    case 4:
        D_00810C64[arg0] += arg1;
        D_00810CAC += 1;
        if (D_00810CAC >= 0x64) {
            D_00810CAC = 0x63;
            D_00810CAE = 0;
        }
        break;

    case 0xC:
    case 0xD:
    case 0xE:
        D_00810C64[arg0] += arg1;
        if (D_00810C70 != 0 && D_00810C71 != 0 && D_00810C72 != 0) {
            D_00810CB0 += 1;
            if (D_00810CB0 >= 0x64) {
                D_00810CB0 = 0x63;
            }
        }
        break;

    default:
        D_00810C64[arg0] += arg1;
        if (D_00810C64[arg0] >= 0x64) {
            D_00810C64[arg0] = 0x63;
        }
        break;
    }
    return 0;
}
