// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 91.73% NEARMISS).
// Fixes read from the original instructions:
//  - The outer mode dispatch is a `switch` on *(int*)(a2+8) (case 0 and case 2
//    sharing one body, then case 1, no default), like the byte-matched sibling
//    handlers func_001B73A0 / func_001B7840, not an if/else chain. The "branch-sense
//    wall" was this wrong control-flow shape.
//  - Every case ends in `break`, and the function returns 0 once after the switch.
//
// Two-phase state handler keyed on the mode word at *(int*)(a2+8) (1, or 2/0). In
// both phases it sub-dispatches on the per-object state byte at *(a1+4) (0/1/2) and
// always returns 0 unless the state-2 arm finds the global gate D_00275BD8 clear, in
// which case it returns 1.
//   mode 2/0 phase: state 0 -> bump the state byte; state 1 -> set gate=1, store
//     (*(a2+8)==2 ? 2 : 1) to D_008106CE, store *(a2+0x14)+0x80 to D_008106CF, bump
//     state; state 2 -> if gate == 0 return 1.
//   mode 1 phase: state 0 -> bump the state byte; state 1 -> set gate D_00275BD8=1,
//     store 1 to D_008106CE, store *(a2+0x14) (no bias) to D_008106CF, bump state;
//     state 2 -> if gate D_00275BD8 == 0 return 1.
// D_00275BD8 is gp-relative (declared scalar under -sdatathreshold 4); D_008106CE/CF
// are over-declared as arrays to force the target's absolute %hi/%lo addressing.
// The inner 0/1/2 dispatch is written as an ascending-case switch so 2.3.3 reverses
// it to the target's 2/1/0 compare order with the case-0 branch-likely fill.
extern unsigned char D_00275BD8;
extern unsigned char D_008106CE[];
extern unsigned char D_008106CF[];

int func_001B7700(int a0, char *a1, char *a2) {
    int v1;
    unsigned char s;

    v1 = *(int *)(a2 + 8);
    switch (v1) {
    case 0:
    case 2:
        s = *(unsigned char *)(a1 + 4);
        switch (s) {
        case 0:
            *(unsigned char *)(a1 + 4) = s + 1;
            break;
        case 1:
            D_00275BD8 = 1;
            if (*(int *)(a2 + 8) == 2) {
                D_008106CE[0] = 2;
            } else {
                D_008106CE[0] = 1;
            }
            D_008106CF[0] = *(unsigned char *)(a2 + 0x14) + 0x80;
            *(unsigned char *)(a1 + 4) = *(unsigned char *)(a1 + 4) + 1;
            break;
        case 2:
            if (D_00275BD8 == 0) {
                return 1;
            }
            break;
        }
        break;
    case 1:
        s = *(unsigned char *)(a1 + 4);
        switch (s) {
        case 0:
            *(unsigned char *)(a1 + 4) = s + 1;
            break;
        case 1:
            D_008106CE[0] = 1;
            D_00275BD8 = 1;
            D_008106CF[0] = *(unsigned char *)(a2 + 0x14);
            *(unsigned char *)(a1 + 4) = *(unsigned char *)(a1 + 4) + 1;
            break;
        case 2:
            if (D_00275BD8 == 0) {
                return 1;
            }
            break;
        }
        break;
    }
    return 0;
}
