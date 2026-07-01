// NEARMISS func_001FDB80  (vram 0x001FDB80, 0x224 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 59.74% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation + inverse-CSE near-miss. Logic fully recovered and correct. Residuals are compiler artifacts: (1) counter/pointer are colored s2/s1 vs the target's s1/s2 (swap cascades through the whole 12-bit loop); (2) mwcc CSEs the loop mask (1<<i) into a callee-saved reg s3 and grows the ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// NEARMISS 60.3% (mwcc 2.3.3). Logic fully recovered; residual is a register-
// allocation permutation (counter/pointer land in s2/s1 vs target s1/s2), the loop
// mask (1<<i) being CSE'd into a callee-saved reg (frame 0x40->0x50 vs target which
// recomputes it in v1 twice), and a dead dsll32/dsrl32 zero-extend of the 0x34 field
// in the state1 path. Body/control-flow are correct.
//
// s0 = &D_002821B0 (a global manager struct). arg0 != 0 is the "init/clear" entry:
// zero fields 0x5C..0x76, then for each of 12 bits set in the mask at s0+0x64, write
// 2 into D_008106B0[0x24 + i] and clear the bit; when the hardware/global byte at
// 0x70003B8F == 2 and i == 0, also call func_001D06E0(&D_008102B0, 0). Then
// func_001FAB80() and return 1. arg0 == 0 is the "tick" entry, a small state machine
// on s0+0x5C (0/1): state 0 tries func_001FD790 to promote to state 1; state 1 reads
// the signed value a1 = s0+0x34, and unless its sign bit is set, dispatches on the
// mode byte D_008106F5 (2: nop; 1: if s0+0x70 == a1 clear D_008106F5; 0: if
// a1 >= s0+0x70 call func_001FD6A0(&D_002821B0, &s0[0x70])). Then bumps s0+0x68,
// calls func_001FD950; on 0 return 0; if s0+0x50 == 0 reset to state 0 and bump
// s0+0x60; otherwise return 1 only when both flags D_00282155 and D_00282156 are 0.
extern void func_001D06E0(int p, int a);
extern void func_001FAB80(void);
extern int func_001FD790(int p);
extern int func_001FD950(int p);
extern void func_001FD6A0(int p, int a);
extern char D_002821B0;
extern char D_008106B0;
extern char D_008102B0;
extern unsigned char D_008106F5;
extern char D_00282155;
extern char D_00282156;

int func_001FDB80(int arg0) {
    char *s0;
    char *s2;
    int i;
    int a1;

    s0 = &D_002821B0;
    if (arg0 != 0) {
        *(int *)(s0 + 0x60) = 0;
        *(int *)(s0 + 0x5C) = 0;
        *(int *)(s0 + 0x68) = 0;
        *(int *)(s0 + 0x6C) = 0;
        *(int *)(s0 + 0x70) = 0;
        *(short *)(s0 + 0x74) = 0;
        *(short *)(s0 + 0x76) = 0;
        s2 = &D_008106B0;
        for (i = 0; i < 0xC; i++) {
            if (*(int *)(s0 + 0x64) & (1 << i)) {
                if (*(unsigned char *)0x70003B8F == 2) {
                    if (i != 0) {
                        goto skipcall;
                    }
                    func_001D06E0((int)&D_008102B0, 0);
                }
            skipcall:
                *(char *)(s2 + 0x24) = 2;
                *(int *)(s0 + 0x64) = *(int *)(s0 + 0x64) & ~(1 << i);
            }
            s2++;
        }
        func_001FAB80();
        return 1;
    }

    if (*(int *)(s0 + 0x5C) == 1) {
        goto state1;
    }
    if (*(int *)(s0 + 0x5C) == 0) {
        if (func_001FD790((int)&D_002821B0)) {
            *(int *)(s0 + 0x5C) = 1;
            goto state1;
        }
        return 0;
    }
    return 0;

state1:
    a1 = *(int *)(s0 + 0x34);
    if (!(a1 & 0x80000000)) {
        switch (D_008106F5) {
        case 2:
            break;
        case 1:
            if (*(int *)(s0 + 0x70) == a1) {
                D_008106F5 = 0;
            }
            break;
        case 0:
            if (a1 >= *(int *)(s0 + 0x70)) {
                func_001FD6A0((int)&D_002821B0, (int)(s0 + 0x70));
            }
            break;
        }
    }
    *(int *)(s0 + 0x68) = *(int *)(s0 + 0x68) + 1;
    if (func_001FD950((int)&D_002821B0) == 0) {
        return 0;
    }
    if (*(unsigned char *)(s0 + 0x50) == 0) {
        *(int *)(s0 + 0x5C) = 0;
        *(int *)(s0 + 0x60) = *(int *)(s0 + 0x60) + 1;
        return 0;
    }
    if (*(char *)&D_00282155 != 0) {
        return 0;
    }
    if (*(char *)&D_00282156 != 0) {
        return 0;
    }
    return 1;
}
