// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 70.93% NEARMISS).
// Fixes read from the original instructions:
//  - The two gates are ONE early return, `if (!gate || busy) return;`: the
//    original keeps a separate return block (branch to the epilogue with the
//    $ra restore in its slot) that only the OR-chain spelling reproduces; the
//    nested `if (gate) { if (busy) return; ... }` was folded into one branch.
//  - The mode-1 guards are one negated && chain ending in the sub-state 0x15
//    test, `if (!(cur < 0x1D && area != 0xB && area != 0xC && area != 0x17 &&
//    sub != 0x15)) return;`: this keeps the three area compares separate (an
//    `||` of `==` tests became a range check) and gives the 0x15 test its own
//    return block, as in the original.
//  - func_001FAAC0 and func_001FAE70 take one argument (their definitions in
//    src/); the extra D_00282178 argument was not passed. func_001FAE70(1)
//    reuses the 1 still held from the D_008104E4 == 1 test.
//
// Gated state-event dispatcher. Runs only when the byte at 0x70003B90 is set;
// bails immediately if 0x70003B8D is set. When D_008104E4 == 1 (a mode flag)
// and the current value D_00282178 is below 0x1D and the level/scene id
// D_00810D38 is none of {0xB,0xC,0x17} and the sub-state D_00810700 is not
// 0x15: unless D_00282178 is already 0x18, it releases channel 0 via
// func_001FAAC0(0) then starts cue 0x18 via func_001FABF0(0,0x18,0xB4,1). In
// the other mode, when D_00282178 == 0x18 it fires func_001FAE70(1).
extern void func_001FAAC0(int);
extern void func_001FABF0(int, int, int, int);
extern void func_001FAE70(int);
extern int D_00282178;
extern unsigned char D_008104E4;
extern unsigned char D_00810700;
extern int D_00810D38;

void func_001FAFD0(void) {
    if (*(unsigned char *)0x70003B90 == 0 || *(unsigned char *)0x70003B8D != 0) {
        return;
    }
    if (D_008104E4 == 1) {
        if (!(D_00282178 < 0x1D && D_00810D38 != 0xB && D_00810D38 != 0xC && D_00810D38 != 0x17 && D_00810700 != 0x15)) {
            return;
        }
        if (D_00282178 != 0x18) {
            func_001FAAC0(0);
            func_001FABF0(0, 0x18, 0xB4, 1);
        }
    } else if (D_00282178 == 0x18) {
        func_001FAE70(1);
    }
}
