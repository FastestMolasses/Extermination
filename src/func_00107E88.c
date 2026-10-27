// COMPILER: eegcc
// CFLAGS: -O2
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 85.64% NEARMISS).
// Fixes read from the original instructions:
//  - func_00107A28 takes the context pointer (a0 is copied into place before the
//    call, as its byte-matched definition shows); func_00107CB8 takes no arguments.
//  - The function is void: the 1 loaded on the early-exit paths is only the
//    constant for the mode == 1 compare. Spelled as early returns plus a void
//    statement tail call (same shape as the byte-matched sibling func_00107FA0),
//    which is what lets ee-gcc emit the sibling jump to func_00107CB8.
//  - The colour-table index is (int)(word >> 5) & 0xF on the 64-bit word.
//
// SDK/MPEG frame setup: fills the 64-bit timing words via func_00107CF0, copies the
// stream's dimensions/timing fields into the device block (D_002412F4 + 0x40), picks
// the table entry from D_002411E0, and, when func_00107A28 accepts the frame and the
// mode is 1, runs the transfer path (func_001084B0 or func_00107AC8 depending on the
// device's 0xB0 flag) followed by func_00107CB8.

extern int func_00107CF0(int a0, int a1, int a2, int a3);
extern int func_00107A28(int a0);
extern void func_001084B0(int a0);
extern void func_00107AC8(int a0);
extern void func_00107CB8(void);

extern int D_002412F4;
extern unsigned int D_002411E0[];

void func_00107E88(int a0) {
    int g = D_002412F4;
    int s1 = *(int *)(g + 0x40);
    func_00107CF0(a0, g + 0x10, g + 0x18, g + 0x20);
    g = D_002412F4;
    *(int *)(s1 + 0x80) = *(int *)(g + 0x10);
    *(unsigned long *)(s1 + 0x88) =
        D_002411E0[(int)(*(long *)(g + 0x20) >> 5) & 0xF];
    *(int *)(s1 + 0xCC) = *(int *)(a0 + 0x5C);
    *(int *)(s1 + 0xD0) = *(int *)(a0 + 0x60);
    *(int *)(s1 + 0xB4) = *(int *)(a0 + 0x44);
    *(int *)(s1 + 0xB8) = *(int *)(a0 + 0x48);
    *(int *)(s1 + 0xBC) = *(int *)(a0 + 0x4C);
    *(int *)(s1 + 0xC0) = *(int *)(a0 + 0x50);
    *(int *)(s1 + 0xC4) = *(int *)(a0 + 0x54);
    *(int *)(s1 + 0xC8) = *(int *)(a0 + 0x58);
    if (func_00107A28(a0) == 0)
        return;
    if (*(int *)(a0 + 0x28) != 1)
        return;
    if (*(int *)(s1 + 0xB0) != 0)
        func_001084B0(a0);
    else
        func_00107AC8(a0);
    func_00107CB8();
}
