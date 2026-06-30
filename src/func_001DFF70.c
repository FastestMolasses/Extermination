// NEARMISS func_001DFF70  (vram 0x001DFF70, 0x178 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 91.97% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation COLORING + minor FP scheduling near-miss (91.97% mwcc 2.3.3). Switch lowers to the exact descending beq cascade with beql; logic fully correct. Residual: entry $a0<->$a1 swap (target: ptr in $a0, const 3 in $a1; mwcc: opposite), which propagates through the body, plus the mul....
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

extern void func_001D2830(int, int, int);
extern void func_001DFE70(char *, int, int, float);
extern char *D_00275670;

void func_001DFF70(void) {
    char *p;
    unsigned char st;
    float v;
    float f3;

    st = *(unsigned char *)(D_00275670 + 0x1F0);
    p = D_00275670 + 0x1F0;
    switch (st) {
    case 0:
        *(float *)(p + 4) = 0.0f;
        *(int *)(p + 0x10) = 0xFF;
        *(int *)(p + 0x14) = 0xDC;
        *(int *)(p + 0x18) = 0xB1;
        *(int *)(p + 0x1C) = 0x40;
        *(unsigned char *)(p + 0) = 2;
        /* fallthrough */
    case 1:
        if (*(unsigned char *)(p + 1) != 1) {
            f3 = *(float *)(p + 4);
            v = f3 + (0.4f * (1.0f - f3) + 0.016666668f);
            *(float *)(p + 4) = v;
            if (!(v < 1.0f)) {
                *(float *)(p + 4) = 1.0f;
                *(unsigned char *)(p + 1) = 1;
            }
        }
        func_001DFE70(p + 0x10, 3, st, 0.42f * *(float *)(p + 4));
        return;
    case 2:
        f3 = *(float *)(p + 4);
        v = f3 + (0.4f * -f3 - 0.016666668f);
        *(float *)(p + 4) = v;
        if (v < 0.0f) {
            *(float *)(p + 4) = 0.0f;
            *(unsigned char *)(p + 0) = 3;
        }
        func_001DFE70(p + 0x10, 3, st, 0.42f * *(float *)(p + 4));
        return;
    case 3:
        func_001D2830(7, 0, st);
        return;
    }
}
