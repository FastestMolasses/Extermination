// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 88.32% NEARMISS).
// Fixes read from the original instructions:
//  - No shared result variable: every failure path is its own `return 0`
//    inside nested ifs (state pointer non-null, then the D_700030B0 case),
//    which lets the state pointer and the result keep their registers.
//  - The 0xFF-index test and the zero-offset test are one `||` condition
//    that assigns the table base and the offset in the test.
//  - The six vertex reads index an array of 12-byte float triples
//    (struct V3) instead of byte-offset arithmetic; that fixes the operand
//    order of the y/z address sums.
//
// Reads a pair of 3-float vectors for the current scratchpad state object
// (*0x700031D0) into arg0 and arg1. If the state is the special D_700030B0
// object, it needs a non-null *0x700031D4 entry in mode 2 (*0x700031D8);
// the entry's high byte at +0xE indexes the offset table at *0x70003250
// (0xFF = none, offset 0 = none), and the six floats at base+offset are
// copied out. Otherwise the state's six shorts index the vertex array at
// *0x700031FC: x of arg0/arg1 from entries 0/1, y from 2/3, z from 4/5.
// Returns 1 when the vectors were written, 0 otherwise.

struct V3 { float x; float y; float z; };

extern char D_700030B0;

int func_00199FA0(char *arg0, char *arg1) {
    int p;
    char *a2;
    int t;
    int base;
    int off;
    float *v;

    p = *(int *)0x700031D0;
    if (p != 0) {
        if (p == (int)&D_700030B0) {
            a2 = *(char **)0x700031D4;
            if (a2 != 0 && *(int *)0x700031D8 == 2) {
                t = (int)*(unsigned short *)(a2 + 0xE) >> 8;
                if (t == 0xFF || (off = *(int *)((base = *(int *)0x70003250) + t * 4 + 4)) == 0) {
                    return 0;
                }
                v = (float *)(base + off);
                *(float *)(arg0 + 0) = v[0];
                *(float *)(arg0 + 4) = v[1];
                *(float *)(arg0 + 8) = v[2];
                *(float *)(arg1 + 0) = v[3];
                *(float *)(arg1 + 4) = v[4];
                *(float *)(arg1 + 8) = v[5];
                return 1;
            }
            return 0;
        }
        *(float *)(arg0 + 0) = (*(struct V3 **)0x700031FC)[*(short *)(p + 0)].x;
        *(float *)(arg1 + 0) = (*(struct V3 **)0x700031FC)[*(short *)(*(int *)0x700031D0 + 2)].x;
        *(float *)(arg0 + 4) = (*(struct V3 **)0x700031FC)[*(short *)(*(int *)0x700031D0 + 4)].y;
        *(float *)(arg1 + 4) = (*(struct V3 **)0x700031FC)[*(short *)(*(int *)0x700031D0 + 6)].y;
        *(float *)(arg0 + 8) = (*(struct V3 **)0x700031FC)[*(short *)(*(int *)0x700031D0 + 8)].z;
        *(float *)(arg1 + 8) = (*(struct V3 **)0x700031FC)[*(short *)(*(int *)0x700031D0 + 0xA)].z;
        return 1;
    }
    return 0;
}
