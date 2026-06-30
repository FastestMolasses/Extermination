// NEARMISS func_001469B0  (vram 0x001469B0, 0x140 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 93.69% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Regalloc + scheduling permutation: control flow and all memory ops byte-structure identical (gp-rel/%hi-%lo split for the two globals matched). Residuals are (1) FP-temp coloring f1/f3 vs f4, (2) slt/slti result colored into $at vs target's $v1, (3) epilogue split where target fills a branch dela...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// NEARMISS 97.6% (mwcc 991202) / 93.7% (mwcc 2.3.3). Logic + control flow
// fully recovered and structurally identical (gp-rel access of D_002753F0
// at threshold 4 with D_0081050C sized > threshold to stay %hi/%lo matches
// the target). Residuals: FP-register coloring (f1/f3 vs f4 on the 0x5C/B0/B8
// math), an slt/slti result colored into $at vs $v1, and an epilogue split
// where the target fills a `b` delay slot with the first `lq` of the
// restore. None is the clean-store nop -> 2.3.3 does not help. Permuter
// (regalloc + scheduling) territory.
//
// Semantics: timer/state machine on the entity at arg1. If 0x64 (timer)
// is running: while flag 0x7F set or height(0x5C) > 100, increment the
// 0x70 counter and at >= 0xB4 ticks clear timer+counter; otherwise reset
// counter. If timer is 0 and flag 0x7F clear: if arg0+0xA bit0 set, arm
// timer=0x12C; else query func_001B3F10 with an aim angle
// (func_001B1470(atan2(0xB0,0xB8) - PI/2), 18.0) and, when it returns
// nonzero, count up to a table-driven threshold (D_002753F0[D_0081050C&3])
// before arming timer=0x12C; on a zero result reset the counter.

extern float func_0011E620(float, float);
extern float func_001B1470(float);
extern int func_001B3F10(unsigned char *, float, float);
extern unsigned char D_002753F0[4];
extern unsigned char D_0081050C[8];

void func_001469B0(unsigned char *arg0, unsigned char *arg1) {
    unsigned char c;
    float *v0;

    if (*(short *)(arg1 + 0x64) != 0) {
        if (*(char *)(arg1 + 0x7F) == 0 && *(float *)(arg1 + 0x5C) <= 100.0f) {
            *(char *)(arg1 + 0x70) = 0;
        } else {
            c = *(unsigned char *)(arg1 + 0x70) + 1;
            *(unsigned char *)(arg1 + 0x70) = c;
            if ((c & 0xFF) >= 0xB4) {
                *(short *)(arg1 + 0x64) = 0;
                *(char *)(arg1 + 0x70) = 0;
            }
        }
    } else if (*(char *)(arg1 + 0x7F) == 0) {
        if (*(unsigned char *)(arg0 + 0xA) & 1) {
            *(short *)(arg1 + 0x64) = 0x12C;
            *(char *)(arg1 + 0x70) = 0;
        } else {
            v0 = (float *)*(int *)(arg0 + 0x14C);
            if (func_001B3F10(arg0, func_001B1470(func_0011E620(v0[0x2C], v0[0x2E]) - 1.5707964f), 18.0f) != 0) {
                c = *(unsigned char *)(arg1 + 0x70) + 1;
                *(unsigned char *)(arg1 + 0x70) = c;
                if ((int)(c & 0xFF) < (int)D_002753F0[D_0081050C[0] & 3]) {
                    return;
                }
                *(short *)(arg1 + 0x64) = 0x12C;
                *(char *)(arg1 + 0x70) = 0;
            } else {
                *(char *)(arg1 + 0x70) = 0;
            }
        }
    }
}
