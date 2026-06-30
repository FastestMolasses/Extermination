// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Engine frame-task sub-state machine (near-exact sibling of func_001ABC60).
// Dispatches on the state byte +9 of the current task slot (slot ptr in
// scratchpad 0x70003B6C); if-chain over states 0..2 (no jr-table).
// State 0 inits the slot (+0x16 u16 = 0x12C, +0xF = 0) and calls
// func_001AEE10(4,0) -- only a0/a1 are set here (2-arg form). State 1 pushes
// a GIF/DMA packet via func_001ABF90 (four 64-bit reg packs), counts down the
// u16 at +0x16, and on underflow runs func_001AEDE0(4,0) + bumps +9. State 2
// pushes the same packet and returns 1 once D_0028A9A0==2. D_0028A9A0
// over-declared short[8] to force absolute addressing.
// Matched 100.0 with mwcc 2.3.3 (mwcps2-2.3.3-000906); 991202 walls at 94.47%
// (clean-store delay-slot-nop residual). 95 instructions, 0 nonmatching vs
// build/expected/func_001ABE10.o.
extern void func_001ABF90(long long, long long, long long, long long);
extern void func_001AEDE0(int, int);
extern void func_001AEE10(int, int);
extern short D_0028A9A0[8];

int func_001ABE10(void) {
    unsigned char *p;
    unsigned char *pp;
    int state;

    p = *(unsigned char **)0x70003B6C;
    pp = p + 9;
    state = p[9];
    switch (state) {
    case 0:
        *pp = *pp + 1;
        (*(unsigned short **)0x70003B6C)[0xB] = 0x12C;
        (*(unsigned char **)0x70003B6C)[0xF] = 0;
        func_001AEE10(4, 0);
        break;
    case 1:
        func_001ABF90(0x21322A00LL | (0x20060006LL << 32),
                      0x21322A40LL | (0x20060086LL << 32),
                      0x21322C00LL | (0x20060206LL << 32),
                      0x21322C40LL | (0x20060286LL << 32));
        {
            unsigned short *q = *(unsigned short **)0x70003B6C;
            unsigned short cnt = q[0xB];
            q[0xB] = cnt - 1;
            if (cnt == 0) {
                func_001AEDE0(4, 0);
                p = *(unsigned char **)0x70003B6C;
                p[9] = p[9] + 1;
            }
        }
        break;
    case 2:
        func_001ABF90(0x21322A00LL | (0x20060006LL << 32),
                      0x21322A40LL | (0x20060086LL << 32),
                      0x21322C00LL | (0x20060206LL << 32),
                      0x21322C40LL | (0x20060286LL << 32));
        if (D_0028A9A0[0] == 2) {
            return 1;
        }
        break;
    }
    return 0;
}
