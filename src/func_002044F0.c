// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// TU_STATIC: func_002041A0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 99.46% NEARMISS).
// Fixes read from the original instructions:
// - The fraction update is the compound `arg0[5] %= 2048`: a plain
//   `arg0[5] = arg0[5] % 2048` coloured the arg0[3] / arg0[5] loads into
//   swapped argument registers.
// - The transfer address is unsigned arithmetic on the base word,
//   `(unsigned int)arg0[0] + (index << 11)`, in both calls; the signed int
//   add coalesced the shifted index instead of the base with the result.
// - The tag pointer is byte arithmetic, `arg0[1] + index * 0x10`, in the
//   first call as in the loop (not a u64 pointer plus index * 2).

// Ring-buffer / DMA-chain submit guarded by a semaphore. WaitSema, then if the
// arg0[0x44] flag is clear just call func_00204080(&D_00273470) and return 0;
// otherwise advance the modular read/write indices (arg0[2]=size, [3]/[4]=
// head/count, [5] packs n<<11 | frac) and issue func_002041A0 transfers
// (mode 3, 0x80), the last one of the loop mode 0. Finally SignalSema. The s0
// compose keeps the low 28 bits | 0x30000000 when the extra first transfer ran,
// and func_00204140 gets it | 0x100.
//
// func_002041A0 is void (out, a, b, c): it stores one 64-bit tag word at `out`.
// n = arg0[5] >> 11 is the loop bound; the original keeps it in $v0 across both
// calls because func_002041A0 is a same-TU leaf (`// TU_STATIC:`), and the
// loop index and cursor stay in $t1 / $t0 for the same reason (frame 0x40).
// func_002040A0's result is its own statement before the modulo, and the
// final `func_00204140(s0 | 0x100)` follows the optional recompose of s0.
extern int WaitSema(int);
extern int SignalSema(int);
extern void func_00204080(void *);
extern int func_002040A0(int *, int);
extern void func_00204140(int);
extern int D_00273470;
static void func_002041A0(unsigned long long *out, unsigned int a, unsigned int b, unsigned int c);
#include "src/func_002041A0.c"

int func_002044F0(int *arg0) {
    int s0;
    int t1;
    int n;
    int t0;
    int s1;
    int hi;
    int a2;
    int r;

    s1 = 0;
    WaitSema(arg0[0x40 / 4]);
    if (arg0[0x44 / 4] == 0) {
        func_00204080(&D_00273470);
        return 0;
    }
    func_00204140(5);
    s0 = *(int *)0x1000B400;
    r = func_002040A0(arg0, *(int *)0x1000B410);
    hi = (r + arg0[2] - arg0[3]) % arg0[2];
    arg0[3] = (arg0[3] + hi) % arg0[2];
    arg0[4] -= hi;
    t0 = (arg0[3] + arg0[4]) % arg0[2];
    n = arg0[5] >> 0xB;
    arg0[5] %= 2048;
    if (n > 0) {
        hi = (arg0[2] + (arg0[3] + arg0[4] - 1)) % arg0[2];
        func_002041A0((unsigned long long *)(arg0[1] + hi * 0x10), (unsigned int)arg0[0] + (hi << 0xB), 3, 0x80);
        s1 = 1;
    }
    t1 = 0;
    if (0 < n) {
        do {
            a2 = (t1 == n - 1) ? 0 : 3;
            func_002041A0((unsigned long long *)(arg0[1] + t0 * 0x10), (unsigned int)arg0[0] + (t0 << 0xB), a2, 0x80);
            t1 += 1;
            t0 = (t0 + 1) % arg0[2];
        } while (t1 < n);
    }
    arg0[4] += n;
    if (arg0[4] != 0) {
        if (s1 != 0) {
            s0 = (s0 & 0x0FFFFFFF) | 0x30000000;
        }
        func_00204140(s0 | 0x100);
    }
    return SignalSema(arg0[0x40 / 4]);
}
