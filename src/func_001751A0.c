// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 97.27% NEARMISS).
// Fixes read from the original instructions:
//  - The body was wrong: the store of the magnitude r to scratchpad 0x70003A24
//    is UNCONDITIONAL (the original stores it in the shared delay slot of the
//    PI/4 compare branch, so it runs on every path), not only in the r <= PI/4
//    arm. The supposed "delay-slot artifact" was this.
//  - Scratchpad accesses 0x70003B8D / 0x70003A20 / 0x70003A24 are relocated
//    externs (idiom-32; all three already in build.py's global _SPAD_SYMS).
//
// Camera/aim-angle classifier. Early-out if the scratchpad gate byte at
// 0x70003B8D is set. Latches the global flag D_00810E57 into self[0x23F]; if it
// latched zero, store self[0x24C] = -1 and return. Otherwise convert two byte
// globals (D_00810E65, D_00810E64) to radians as PI * ((unsigned)b / 256.0),
// take func_0011DE90 (cos) of each into self[0x244]/self[0x248], then form a
// heading via func_0011E620 (atan2): r = atan2(-self[0x248], self[0x244]).
// Offset it by PI and the global bias D_008106A0, wrap through func_001B1470,
// subtract the current yaw self[0xC4], wrap again, and stash the result at the
// scratchpad float 0x70003A20. func_0011DF78 maps that to a magnitude r, which
// is always stored at scratchpad 0x70003A24, then:
//   r <= PI/4        -> self[0x24C] = 0
//   PI/4 < r < 3PI/4 -> self[0x24C] = (scratch 0x70003A20 < 0) ? 2 : 3
//   r >= 3PI/4       -> self[0x24C] = 1
extern volatile unsigned char D_70003B8D;
extern volatile float D_70003A20;
extern float D_70003A24;
extern float func_0011DE90(float a);
extern float func_0011E620(float a, float b);
extern float func_001B1470(float a);
extern float func_0011DF78(float a);
extern unsigned char D_00810E57;
extern unsigned char D_00810E64;
extern unsigned char D_00810E65;
extern float D_008106A0;

void func_001751A0(unsigned char *self) {
    float a64, a65;
    float r;

    if (D_70003B8D != 0) {
        return;
    }

    self[0x23F] = D_00810E57;
    if (self[0x23F] == 0) {
        *(int *)(self + 0x24C) = -1;
        return;
    }

    a65 = 3.14159265f * ((float)(unsigned int)D_00810E65 / 256.0f);
    a64 = 3.14159265f * ((float)(unsigned int)D_00810E64 / 256.0f);

    *(float *)(self + 0x244) = func_0011DE90(a64);
    *(float *)(self + 0x248) = func_0011DE90(a65);

    r = func_0011E620(-*(float *)(self + 0x248), *(float *)(self + 0x244));
    r = func_001B1470((3.14159265f + r) + D_008106A0);
    r = func_001B1470(r - *(float *)(self + 0xC4));
    D_70003A20 = r;
    r = func_0011DF78(r);

    D_70003A24 = r;
    if (r <= 0.785398185f) {
        *(int *)(self + 0x24C) = 0;
        return;
    }
    if (r >= 2.35619449f) {
        *(int *)(self + 0x24C) = 1;
    } else {
        if (D_70003A20 < 0.0f) {
            *(int *)(self + 0x24C) = 2;
        } else {
            *(int *)(self + 0x24C) = 3;
        }
    }
}
