// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// HUD/overlay frame routine, twin of func_00229C90 with a richer func_001FECB0
// result handler. Same fixed GS/GIF setup (func_00207D00, one 64-bit-packed
// func_00207E40, func_00207F80, four func_001FCBD0 with color 0x70808080), the
// per-mode func_00207F80 keyed on +9, and the same D_00810E74 flag reactions
// (0x40 finish; 0x20/0x10 abort returning 2; 0x2000 arm / 0x8000 disarm
// func_0020CDA0). After the func_001FE9A0 gate it steps func_001FECB0 (selector
// from +0x24 bit 0) and dispatches on the result: case 0 records readiness in
// the per-selector latch (+0x2C for the bit-set path, +0x28 for the clear path)
// = 1 when +0x58==2 else 0; cases -2/-3 clear the state on +0x58==2 else clear
// the latch; default (positive results) clears the state only when BOTH latches
// (+0x28 and +0x2C) are zero, then clears this selector's latch. Each branch
// pulses func_001FE8D0; any non -1 result bumps the frame counter at +0x24.
//
// Built with mwcc 2.3.3: the result handler is a switch with cases ordered
// (0; -1; -2 then -3 grouped; default) so CW's sequential beq dispatch order
// (-3, -2, -1, 0) and block placement reproduce exactly -- including the precise
// -3-before--2 comparison order. 64-bit func_00207E40 packet 0x2005C805DD422E00;
// func_001FCBD0 5th color arg; func_001FECB0 5th (t0=0) arg. 991202 reaches
// 86.4%. Verified objdiff 100.0 vs build/expected/func_0022A020.o.
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

int func_0022A020(char *arg0) {
    int r;

    func_00207D00(1, 0);
    func_00207E40(1, 0x8800, 0x8300, 0x80, 0x80, 0x80808080, 0x2005C805DD422E00ULL);
    func_00207F80(1, 0x8800, 0x84C0, 0x9000, 0x86C0, 0x80010101);
    func_001FCBD0(0x3C, 0x32, 7, 0xC, 0x70808080);
    func_001FCBD0(0x3C, 0x5A, 7, 7, 0x70808080);
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
        case 0:
            if (*(int *)(arg0 + 0x58) == 2) {
                *(int *)(arg0 + 0x2C) = 1;
            } else {
                *(int *)(arg0 + 0x2C) = 0;
            }
            func_001FE8D0();
            break;
        case -1:
            break;
        case -2:
        case -3:
            if (*(int *)(arg0 + 0x58) == 2) {
                *(signed char *)(arg0 + 0) = 0;
            } else {
                *(int *)(arg0 + 0x2C) = 0;
            }
            func_001FE8D0();
            break;
        default:
            if (*(int *)(arg0 + 0x28) == 0 && *(int *)(arg0 + 0x2C) == 0) {
                *(signed char *)(arg0 + 0) = 0;
            }
            *(int *)(arg0 + 0x2C) = 0;
            func_001FE8D0();
            break;
        }
    } else {
        r = func_001FECB0(0, 0, arg0 + 0x58, 0, 0);
        switch (r) {
        case 0:
            if (*(int *)(arg0 + 0x58) == 2) {
                *(int *)(arg0 + 0x28) = 1;
            } else {
                *(int *)(arg0 + 0x28) = 0;
            }
            func_001FE8D0();
            break;
        case -1:
            break;
        case -2:
        case -3:
            if (*(int *)(arg0 + 0x58) == 2) {
                *(signed char *)(arg0 + 0) = 0;
            } else {
                *(int *)(arg0 + 0x28) = 0;
            }
            func_001FE8D0();
            break;
        default:
            if (*(int *)(arg0 + 0x28) == 0 && *(int *)(arg0 + 0x2C) == 0) {
                *(signed char *)(arg0 + 0) = 0;
            }
            *(int *)(arg0 + 0x28) = 0;
            func_001FE8D0();
            break;
        }
    }
    if (r != -1) {
        *(int *)(arg0 + 0x24) = *(int *)(arg0 + 0x24) + 1;
    }
    return 0;
}
