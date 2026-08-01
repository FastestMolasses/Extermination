// NEARMISS func_001E55F0  (vram 0x001E55F0, 0x4CC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 93.77% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation-order permutation: target keeps the actor+0x1F0 pointer in $s2 and the trigger-flag accumulator in $s1 throughout; this compile has mwcc pick $s0/$s1 instead. Body, control flow, and instruction count are exact (the fv0f/ft0 FP register-name differences are cosmetic, not count...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

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
// D_0028A9A0==2, sets state to 3 and returns early.
extern unsigned char D_70003B8D;                    /* PS2 scratchpad @ 0x70003B8D */

extern float func_0011DF78(float);
extern int func_00122BB8(void);
extern char func_00128250(int, float);
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
    char *s2;
    int flags;
    char *p;
    float f;
    int v0;
    int i;

    s2 = arg0 + 0x1F0;
    switch (*(unsigned char *)(arg0 + 4)) {
    case 0:
        *(int *)(s2 + 0x3C) = func_00122BB8();
        p = s2;
        for (i = 0; i < 6; i++) {
            *(float *)(p + 0) = 1.0f + (float)func_00122BB8() / 2147483647.0f;
            *(float *)(p + 0x18) = 0.2f * ((float)func_00122BB8() / 2147483647.0f);
            p += 4;
        }
        *(float *)(s2 + 0x30) = 48.0f + 16.0f * ((float)func_00122BB8() / 2147483647.0f);
        *(float *)(s2 + 0x34) = 48.0f + 16.0f * ((float)func_00122BB8() / 2147483647.0f);
        *(float *)(s2 + 0x38) = 0.003f;
        *(int *)(s2 + 0x40) = func_00122BB8() % 10 + 0xA;
        *(unsigned char *)(arg0 + 4) = 1;
        /* fallthrough */
    case 1:
        v0 = func_001B0070();
        if (v0 & 0x0E000070) {
            f = *(float *)(s2 + 0x34) - *(float *)(s2 + 0x30);
            *(float *)(s2 + 0x30) = *(float *)(s2 + 0x30) + f * *(float *)(s2 + 0x38);
            if (func_0011DF78(f) < 3.0f) {
                if (v0 & 0x02000010) {
                    *(float *)(s2 + 0x34) = 60.0f + 15.0f * ((float)func_00122BB8() / 2147483647.0f);
                }
                if (v0 & 0x04000020) {
                    *(float *)(s2 + 0x34) = 65.0f + 20.0f * ((float)func_00122BB8() / 2147483647.0f);
                }
                if (v0 & 0x08000040) {
                    *(float *)(s2 + 0x34) = 65.0f + 30.0f * ((float)func_00122BB8() / 2147483647.0f);
                    *(float *)(s2 + 0x38) = 0.003f;
                    *(int *)(s2 + 0x40) = *(int *)(s2 + 0x40) - 1;
                    if (*(int *)(s2 + 0x40) < 0) {
                        *(int *)(s2 + 0x40) = func_00122BB8() % 10 + 0xA;
                        *(float *)(s2 + 0x34) = 127.0f;
                        *(float *)(s2 + 0x38) = 0.05f;
                    }
                }
            }
            if (D_70003B8D != 0) {
                if (!(*(float *)(s2 + 0x34) <= 90.0f)) {
                    *(float *)(s2 + 0x34) = 90.0f;
                }
            }
            flags = 0;
            if (v0 & 0x0C000060) {
                flags = 1;
            }
            if (((D_00810700[0] << 8) + D_00810701[0]) == 0x1500) {
                flags |= 2;
            }
            D_008106BF[0] = func_00128250((D_00810700[0] << 8), *(float *)(s2 + 0x30));
            f = *(float *)(s2 + 0x30) / 127.0f;
            if (!(*(float *)(s2 + 0x30) <= 0.0f)) {
                p = *(char **)(D_00275670 + 0x1C);
                if (flags & 1) {
                    func_001E5AC0(arg0, flags, *(int *)(s2 + 0x3C), f);
                } else {
                    func_001E67C0(arg0, flags, *(int *)(s2 + 0x3C), f);
                }
                *(char *)(*(char **)(D_00275670 + 0x1C) + 3) = 0x60;
                *(int *)(*(char **)(D_00275670 + 0x1C) + 4) = 0;
                *(short *)(*(char **)(D_00275670 + 0x1C) + 0) = 0;
                *(char **)(D_00275670 + 0x1C) = *(char **)(D_00275670 + 0x1C) + 0x10;
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
