// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
// SPAD: 0x70003B8D

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 93.77% NEARMISS).
// Fixes read from the original instructions:
// - func_00128250 takes one float (its definition in src/), not (int, float).
// - The debug gate byte at 0x70003B8D is a relocated scratchpad extern (idiom-32),
//   declared as an array so it is addressed absolutely rather than $gp-relative.
// - The mode accumulator starts at 0 at the top of state 1 and is OR-ed with 1;
//   the 0x1500 test is a ternary (`!= 0x1500 ? flags : flags | 2`), which gives
//   the original's branch-likely shape.
// - The blend is `+=`, the noise-table loop initialises i and p together, and the
//   display-list header (D_00275670) is read once into a local after the call.
// - The saved-register assignment needed the state-0 seed to pass through the
//   same local as the trigger-flag word (found by the permuter), plus the
//   declaration order flags, i, val, p, blk.
//
// Actor per-frame tick: state 0 seeds a 6-entry float[2] noise table plus a
// 0x30/0x34/0x38/0x40 blend-rate/target/timer group and advances to state 1.
// State 1 (fallthrough) polls func_001B0070() flags: blends field 0x30 toward
// 0x34 by rate 0x38, and while the delta magnitude is small, re-rolls the
// target 0x34 (and rate/timer 0x38/0x40) on three escalating trigger-flag
// groups. A debug gate (byte at 0x70003B8D) clamps 0x34 to 90. Combines
// trigger flags into a local mode (bit0/bit1), calls func_00128250 to sample
// the blended value into D_008106BF, and — if the blended value is positive —
// dispatches to func_001E5AC0 (mode&1) or func_001E67C0 (else) with the
// ratio value/127, then recycles the display-list node at D_00275670+0x1C
// (writes header byte+3=0x60, word+4=0, half+0=0, advances by 0x10) and frees
// the previous node via func_001D2DE0. Finally, if D_008106B8==2 and
// D_0028A9A0==2, sets state to 3.
// `val` first holds the state-0 seed written to +0x3C, then the
// func_001B0070() trigger-flag word.
extern unsigned char D_70003B8D[];                  /* PS2 scratchpad @ 0x70003B8D (relocated extern) */

extern float func_0011DF78(float);
extern int func_00122BB8(void);
extern unsigned int func_00128250(float);
extern void func_001AFC10(char *);
extern int func_001B0070(void);
extern void func_001D2DE0(int, char *);
extern void func_001E5AC0(char *, int, int, float);
extern void func_001E67C0(char *, int, int, float);
extern char *D_00275670;
extern short D_0028A9A0[64];
extern unsigned char D_008106B8[64];
extern char D_008106BF[64];
extern unsigned char D_00810700[64];
extern unsigned char D_00810701[64];

void func_001E55F0(char *arg0) {
    int flags;
    int i;
    int val;
    char *p;
    char *blk;
    char *dl;
    float f;

    blk = arg0 + 0x1F0;
    switch (*(unsigned char *)(arg0 + 4)) {
    case 0:
        val = func_00122BB8();
        *(int *)(blk + 0x3C) = val;
        for (i = 0, p = blk; i < 6; i++) {
            *(float *)(p + 0) = 1.0f + (float)func_00122BB8() / 2147483647.0f;
            *(float *)(p + 0x18) = 0.2f * ((float)func_00122BB8() / 2147483647.0f);
            p += 4;
        }
        *(float *)(blk + 0x30) = 48.0f + 16.0f * ((float)func_00122BB8() / 2147483647.0f);
        *(float *)(blk + 0x34) = 48.0f + 16.0f * ((float)func_00122BB8() / 2147483647.0f);
        *(float *)(blk + 0x38) = 0.003f;
        *(int *)(blk + 0x40) = func_00122BB8() % 10 + 0xA;
        *(unsigned char *)(arg0 + 4) = 1;
        /* fallthrough */
    case 1:
        flags = 0;
        val = func_001B0070();
        if (val & 0x0E000070) {
            f = *(float *)(blk + 0x34) - *(float *)(blk + 0x30);
            *(float *)(blk + 0x30) += f * *(float *)(blk + 0x38);
            if (func_0011DF78(f) < 3.0f) {
                if (val & 0x02000010) {
                    *(float *)(blk + 0x34) = 60.0f + 15.0f * ((float)func_00122BB8() / 2147483647.0f);
                }
                if (val & 0x04000020) {
                    *(float *)(blk + 0x34) = 65.0f + 20.0f * ((float)func_00122BB8() / 2147483647.0f);
                }
                if (val & 0x08000040) {
                    *(float *)(blk + 0x34) = 65.0f + 30.0f * ((float)func_00122BB8() / 2147483647.0f);
                    *(float *)(blk + 0x38) = 0.003f;
                    *(int *)(blk + 0x40) = *(int *)(blk + 0x40) - 1;
                    if (*(int *)(blk + 0x40) < 0) {
                        *(int *)(blk + 0x40) = func_00122BB8() % 10 + 0xA;
                        *(float *)(blk + 0x34) = 127.0f;
                        *(float *)(blk + 0x38) = 0.05f;
                    }
                }
            }
            if (D_70003B8D[0] != 0) {
                if (!(*(float *)(blk + 0x34) <= 90.0f)) {
                    *(float *)(blk + 0x34) = 90.0f;
                }
            }
            if (val & 0x0C000060) {
                flags |= 1;
            }
            flags = (((D_00810700[0] << 8) + D_00810701[0]) != 0x1500) ? flags : flags | 2;
            D_008106BF[0] = func_00128250(*(float *)(blk + 0x30));
            f = *(float *)(blk + 0x30) / 127.0f;
            if (!(*(float *)(blk + 0x30) <= 0.0f)) {
                p = *(char **)(D_00275670 + 0x1C);
                if (flags & 1) {
                    func_001E5AC0(arg0, flags, *(int *)(blk + 0x3C), f);
                } else {
                    func_001E67C0(arg0, flags, *(int *)(blk + 0x3C), f);
                }
                dl = D_00275670;
                *(char *)(*(char **)(dl + 0x1C) + 3) = 0x60;
                *(int *)(*(char **)(dl + 0x1C) + 4) = 0;
                *(short *)(*(char **)(dl + 0x1C) + 0) = 0;
                *(char **)(dl + 0x1C) = *(char **)(dl + 0x1C) + 0x10;
                if (p != 0) {
                    func_001D2DE0(0, p);
                }
            }
            if (D_008106B8[0] == 2 && D_0028A9A0[0] == 2) {
                *(char *)(arg0 + 4) = 3;
            }
        }
        break;
    case 2:
    case 3:
        func_001AFC10(arg0);
        break;
    }
}
