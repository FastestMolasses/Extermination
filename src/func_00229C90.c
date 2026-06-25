// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// HUD/overlay frame routine. Issues the fixed GS/GIF setup sequence
// (func_00207D00, one 64-bit-packed func_00207E40 register write, func_00207F80,
// four func_001FCBD0 register writes each carrying the 0x70808080 color in t0),
// then a per-mode func_00207F80 selected by the mode byte at +9. Reacts to the
// global input/flag word D_00810E74: bit 0x40 -> func_0020CD40 + finish (state
// byte +0: mode0 sets 3 returns 1, mode1 sets 0 returns 2); bit 0x20/0x10 ->
// func_0020CD60, clear state, return 2. bit 0x2000 arms func_0020CDA0 (sets +9),
// bit 0x8000 disarms it (clears +9). Finally, gated again by func_001FE9A0, it
// steps func_001FECB0 (selector chosen by +0x24 bit 0); on a terminal result
// (0/-2/-3) it clears the state when +0x58==2 and pulses func_001FE8D0, and on
// any non -1 result increments the frame counter at +0x24.
//
// Built with mwcc 2.3.3: the func_001FECB0 result handler MUST be a switch
// (case -1; cases -3/-2/0 grouped; default) so CW's sequential beq dispatch
// (0, -2, -3, -1) is reproduced rather than the range-test mwcc emits for an
// if-chain; the beql mode dispatch and the 0x40/else-if(0x20|0x10) shape also
// matter. The 64-bit func_00207E40 packet is 0x2005C805DD422E00. func_00207E40
// takes (a0..a3, t0, t1, u64); func_001FCBD0 carries a 5th color arg;
// func_001FECB0 carries a 5th (t0=0) arg. 991202 reaches 84.7% (delay-slot/
// branch-lowering wall). Verified objdiff 100.0 vs build/expected/func_00229C90.o.
extern void func_00207D00(int, int);
extern void func_00207E40(int, int, int, int, int, int, unsigned long long);
extern void func_00207F80(int, int, int, int, int, int);
extern void func_001FCBD0(int, int, int, int, unsigned int);
extern void func_0020CD40(void);
extern void func_0020CD60(void);
extern void func_0020CDA0(void);
extern void func_001FE8D0(void);
extern int func_001FE9A0(int, char *, char *);
extern int func_001FECB0(int, int, char *, int, int);
extern unsigned short D_00810E74;

int func_00229C90(char *arg0) {
    int r;

    func_00207D00(1, 0);
    func_00207E40(1, 0x8800, 0x8300, 0x80, 0x80, 0x80808080, 0x2005C805DD422E00ULL);
    func_00207F80(1, 0x8800, 0x84C0, 0x9000, 0x86C0, 0x80010101);
    func_001FCBD0(0x3C, 0x32, 7, 0xB, 0x70808080);
    func_001FCBD0(0x3C, 0x5A, 7, 8, 0x70808080);
    func_001FCBD0(0x3C, 0x96, 7, 0x20, 0x70808080);
    func_001FCBD0(0x8C, 0xAA, 7, 2, 0x70808080);
    switch (*(unsigned char *)(arg0 + 9)) {
    case 0:
        func_00207F80(1, 0x77E0, 0x83C0, 0x78A0, 0x8420, 0x80CE6000);
        break;
    case 1:
        func_00207F80(1, 0x7CD0, 0x83C0, 0x7D90, 0x8420, 0x80CE6000);
        break;
    }
    if (D_00810E74 & 0x40) {
        func_0020CD40();
        func_001FE8D0();
        switch (*(unsigned char *)(arg0 + 9)) {
        case 0:
            *(signed char *)(arg0 + 0) = 3;
            return 1;
        case 1:
            *(signed char *)(arg0 + 0) = 0;
            return 2;
        }
    } else if ((D_00810E74 & 0x20) || (D_00810E74 & 0x10)) {
        func_0020CD60();
        func_001FE8D0();
        *(signed char *)(arg0 + 0) = 0;
        return 2;
    }
    if ((D_00810E74 & 0x2000) && *(unsigned char *)(arg0 + 9) == 0) {
        *(unsigned char *)(arg0 + 9) = 1;
        func_0020CDA0();
    }
    if (D_00810E74 & 0x8000) {
        if (*(unsigned char *)(arg0 + 9) != 0) {
            *(unsigned char *)(arg0 + 9) = 0;
            func_0020CDA0();
        }
    }
    if (func_001FE9A0(1, arg0 + 0x50, arg0 + 0x54) == 0) {
        return 0;
    }
    if (*(int *)(arg0 + 0x24) & 1) {
        r = func_001FECB0(1, 0, arg0 + 0x58, 0, 0);
        switch (r) {
        case -1:
            break;
        case -3:
        case -2:
        case 0:
            if (*(int *)(arg0 + 0x58) == 2) {
                *(signed char *)(arg0 + 0) = 0;
            }
            func_001FE8D0();
            break;
        default:
            func_001FE8D0();
            break;
        }
    } else {
        r = func_001FECB0(0, 0, arg0 + 0x58, 0, 0);
        switch (r) {
        case -1:
            break;
        case -3:
        case -2:
        case 0:
            if (*(int *)(arg0 + 0x58) == 2) {
                *(signed char *)(arg0 + 0) = 0;
            }
            func_001FE8D0();
            break;
        default:
            func_001FE8D0();
            break;
        }
    }
    if (r != -1) {
        *(int *)(arg0 + 0x24) = *(int *)(arg0 + 0x24) + 1;
    }
    return 0;
}
