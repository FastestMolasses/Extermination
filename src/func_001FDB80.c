// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 60.52% NEARMISS).
// Fixes read from the original instructions:
// - the mask loop is the byte-matched sibling func_001FD950's do-while
//   (record pointer, then bit counter; unsigned mask clear);
// - the state dispatch is one switch on +0x5C: case 0 promotes to state 1 (if/else,
//   else returns 0) and falls into case 1; any other state returns 1 (the original
//   reuses the 1 the state compare left in the result register);
// - the +0x34 index is a 64-bit local: the original builds the 31-bit index with a
//   64-bit shift pair and passes the full register to func_001FD6A0 without
//   truncating it (so the call takes an unsigned long long here);
// - the mode-0 test is written +0x70 <= index (its compare result goes to the
//   assembler temporary as in the original);
// - the tail is nested: only when func_001FD950 returns nonzero and +0x50 == 0 is
//   the state reset to 0 and +0x60 bumped; with +0x50 set, 1 is returned only when
//   both flags D_00282155 and D_00282156 are 0; everything else returns 0.
//
// s0 = &D_002821B0 (a global manager struct). arg0 != 0 is the "init/clear" entry:
// zero fields 0x5C..0x76, then for each of 12 bits set in the mask at s0+0x64, write
// 2 into D_008106B0[0x24 + i] and clear the bit; when the scratchpad game-mode byte at
// 0x70003B8F == 2 and i == 0, first call func_001D06E0(&D_008102B0, 0). Then
// func_001FAB80() and return 1. arg0 == 0 is the "tick" entry, a small state machine
// on s0+0x5C: state 0 tries func_001FD790 to promote to state 1 (else returns 0);
// state 1 reads the signed value s0+0x34, and unless its sign bit is set, dispatches
// on the mode byte D_008106F5 (2: nothing; 1: if s0+0x70 == value clear D_008106F5;
// 0: if value >= s0+0x70 call func_001FD6A0(value & 0x7FFFFFFF, &s0[0x70])). Then
// bumps s0+0x68 and calls func_001FD950 (see above for the tail). Other states
// return 1.
extern void func_001D06E0(int p, int a);
extern void func_001FAB80(void);
extern int func_001FD790(int p);
extern int func_001FD950(int p);
extern int func_001FD6A0(unsigned long long idx, int *out);
extern char D_002821B0;
extern char D_008106B0;
extern char D_008102B0;
extern unsigned char D_008106F5;
extern char D_00282155;
extern char D_00282156;

int func_001FDB80(int arg0) {
    char *s0;
    int bit;
    char *rec;
    long a1;
    unsigned long long idx;

    s0 = &D_002821B0;
    if (arg0 != 0) {
        *(int *)(s0 + 0x60) = 0;
        *(int *)(s0 + 0x5C) = 0;
        *(int *)(s0 + 0x68) = 0;
        *(int *)(s0 + 0x6C) = 0;
        *(int *)(s0 + 0x70) = 0;
        *(short *)(s0 + 0x74) = 0;
        *(short *)(s0 + 0x76) = 0;
        rec = &D_008106B0;
        bit = 0;
        do {
            if (*(int *)(s0 + 0x64) & (1 << bit)) {
                if (*(unsigned char *)0x70003B8F == 2) {
                    if (bit == 0) {
                        func_001D06E0((int)&D_008102B0, 0);
                    }
                }
                *(char *)(rec + 0x24) = 2;
                *(unsigned int *)(s0 + 0x64) &= ~(1 << bit);
            }
            bit += 1;
            rec += 1;
        } while (bit < 0xC);
        func_001FAB80();
        return 1;
    }

    switch (*(int *)(s0 + 0x5C)) {
    case 0:
        if (func_001FD790((int)&D_002821B0)) {
            *(int *)(s0 + 0x5C) = 1;
        } else {
            return 0;
        }
    case 1:
        a1 = *(int *)(s0 + 0x34);
        if (!((int)a1 & 0x80000000)) {
            idx = (unsigned long long)a1 << 0x21;
            idx >>= 0x21;
            switch (D_008106F5) {
            case 0:
                if (*(int *)(s0 + 0x70) <= a1) {
                    func_001FD6A0(idx, (int *)(s0 + 0x70));
                }
                break;
            case 1:
                if (*(int *)(s0 + 0x70) == a1) {
                    D_008106F5 = 0;
                }
                break;
            case 2:
                break;
            }
        }
        *(int *)(s0 + 0x68) = *(int *)(s0 + 0x68) + 1;
        if (func_001FD950((int)&D_002821B0) != 0) {
            if (*(unsigned char *)(s0 + 0x50) != 0) {
                if (D_00282155 == 0 && D_00282156 == 0) {
                    return 1;
                }
                return 0;
            }
            *(int *)(s0 + 0x5C) = 0;
            *(int *)(s0 + 0x60) = *(int *)(s0 + 0x60) + 1;
        }
        return 0;
    }
    return 1;
}
